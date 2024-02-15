#ifndef NODEPP_TCP
#define NODEPP_TCP

/*────────────────────────────────────────────────────────────────────────────*/

#include "socket.h"
#include "poll.h"
#include "dns.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

/*────────────────────────────────────────────────────────────────────────────*/

class tcp_t {
protected:

    struct NODE {
        int                       state = 0;
        bool                      chck  = 1;
        agent_t                   agent;
        poll_t                    poll ;
        function_t<void,socket_t> func ;
    };  ptr_t<NODE> obj;
    
    /*─······································································─*/

    void init_poll_loop( ptr_t<const tcp_t>& inp ) const noexcept { process::poll::add([=](){
        if( inp->is_closed() ){ return -1; } if( inp->obj->poll.emit() != -1 ) { auto x = inp->obj->poll.get_last_poll();
            if( x[0] == 0 ){ socket_t cln(x[1]); cln.set_sockopt(inp->obj->agent); inp->onSocket.emit(cln); inp->obj->func(cln); }
            if( x[0] == 1 ){ socket_t cln(x[1]); cln.set_sockopt(inp->obj->agent); inp->onSocket.emit(cln); inp->obj->func(cln); }
        #if _KERNEL == NODEPP_KERNEL_WINDOWS
            if( x[0] ==-1 ){ ::closesocket(x[1]); }
        #else
            if( x[0] ==-1 ){ ::close(x[1]); }
        #endif
        }   return 1;
    }); }
    
public: tcp_t() noexcept : obj( new NODE() ) {}

    event_t<socket_t> onConnect;
    event_t<socket_t> onSocket;
    event_t<>         onClose;
    event_t<except_t> onError;
    event_t<socket_t> onOpen;
    
    /*─······································································─*/
    
    tcp_t( decltype(NODE::func) _func, agent_t* opt=nullptr ) noexcept : obj( new NODE() )
         { obj->func=_func; obj->agent=opt==nullptr?agent_t():*opt;  }
    
    /*─······································································─*/
    
    void     close() const noexcept { if( obj->state<0 ){ return; } obj->state=-1; onClose.emit(); }
    bool is_closed() const noexcept { return obj == nullptr ? 1 : obj->state < 0; }
    
    /*─······································································─*/

    void poll( bool chck ) const noexcept { obj->chck = chck; }
    
    /*─······································································─*/

    void listen( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto inp = type::bind( this );
        if( dns::lookup(host).empty() ){ process::error(onError,"dns couldn't get ip"); close(); return; }

        socket_t *sk = new socket_t; 
                  sk->PROT = IPPROTO_TCP;
                  sk->socket( dns::lookup(host), port ); 
        
        if(   sk->bind()  < 0 ){ process::error(onError,"Error while binding TCP");   close(); delete sk; return; }
        if( sk->listen()  < 0 ){ process::error(onError,"Error while listening TCP"); close(); delete sk; return; }
        if( obj->chck == true ){ init_poll_loop( inp ); }

        onOpen.emit(*sk); if( cb != nullptr ){ (*cb)(*sk); }
        
        process::task::add([=](){
            static int _accept = 0; 
        coStart

            while( sk != nullptr ){ _accept = sk->_accept();
                if( inp->is_closed() || !sk->is_available() )
                  { break; } elif ( _accept != -2 )
                  { break; } coYield(1);
            }
            
            if( _accept == -1 ){ process::error(inp->onError,"Error while accepting TCP"); coGoto(2); }
            elif ( !sk->is_available() || inp->is_closed() ){ coGoto(2); }
            elif ( inp->obj->chck == true ){ inp->obj->poll.push_read(_accept); coGoto(0); }
            else { socket_t cln( _accept ); if( cln.is_available() ){ 
                   process::poll::add([=]( socket_t cln ){
                        cln.set_sockopt( inp->obj->agent ); 
                        inp->onSocket.emit( cln ); 
                        inp->obj->func( cln ); 
                        return -1;
                   }, cln );
            } coGoto(0); }

            coYield(2); inp->close(); delete sk; 
        
        coStop
        });

    }

    void listen( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         listen( host, port, &cb ); 
    }

    /*─······································································─*/

    void connect( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto inp = type::bind( this );
        if( dns::lookup(host).empty() )
          { process::error(onError,"dns couldn't get ip"); close(); return; }

        socket_t sk = socket_t(); 
                 sk.PROT = IPPROTO_TCP;
                 sk.socket( dns::lookup(host), port );
                 sk.set_sockopt( obj->agent );

        if( sk.connect() < 0 ){ process::error(onError,"Error while connecting TCP"); close(); return; }
        if( cb != nullptr ){ (*cb)(sk); } sk.onClose.on([=](){ inp->close(); });
        onOpen.emit(sk); sk.onOpen.emit(); onSocket.emit(sk); obj->func(sk);
    }

    void connect( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         connect( host, port, &cb ); 
    }

};

/*────────────────────────────────────────────────────────────────────────────*/

namespace tcp {

    tcp_t server( const tcp_t& server ){ server.onSocket([=]( socket_t cln ){
        ptr_t<_file_::read> _read = new _file_::read;
        cln.onDrain.once([=](){ cln.free(); });
        cln.busy();

        server.onConnect.once([=]( socket_t cln ){ process::poll::add([=](){
            if(!cln.is_available() ) { cln.close(); return -1; }
            if((*_read)(&cln)==1 )   { return 1; }
            if(  _read->c  <=  0 )   { return 1; }
            cln.onData.emit(_read->y); return 1;
        }) ; });

        process::task::add([=](){
            server.onConnect.emit(cln); return -1;
        });

    }); server.poll( false ); return server; }

    /*─······································································─*/

    tcp_t server( agent_t* opt=nullptr ){
        auto server = tcp_t( [=]( socket_t /*unused*/ ){}, opt );
        tcp::server( server ); return server; 
    }

    /*─······································································─*/

    tcp_t clnent( const tcp_t& clnent ){ clnent.onOpen.once([=]( socket_t cln ){
        ptr_t<_file_::read> _read = new _file_::read;
        cln.onDrain.once([=](){ cln.free(); });
        cln.busy();

        process::poll::add([=](){
            if(!cln.is_available() ) { cln.close(); return -1; }
            if((*_read)(&cln)==1 )   { return 1; }
            if(  _read->c  <=  0 )   { return 1; }
            cln.onData.emit(_read->y); return 1;
        }); 

    }); return clnent; }

    /*─······································································─*/

    tcp_t clnent( agent_t* opt=nullptr ){
        auto clnent = tcp_t( [=]( socket_t /*unused*/ ){}, opt );
        tcp::clnent( clnent ); return clnent; 
    }

}

/*────────────────────────────────────────────────────────────────────────────*/

}

#endif