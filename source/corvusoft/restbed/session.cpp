/*
 * Copyright (c) 2013, 2014, 2015 Corvusoft
 */

//System Includes

//Project Includes
#include "corvusoft/restbed/session.hpp"
#include "corvusoft/restbed/response.hpp"
#include "corvusoft/restbed/context_value.hpp"
#include "corvusoft/restbed/session_manager.hpp"
#include "corvusoft/restbed/detail/socket_impl.hpp"
#include "corvusoft/restbed/detail/session_impl.hpp"

//External Includes
#include <asio.hpp>

//System Namespaces
using std::set;
using std::string;
using std::function;
using std::multimap;
using std::shared_ptr;
using std::runtime_error;
using std::chrono::hours;
using std::chrono::minutes;
using std::chrono::seconds;
using std::invalid_argument;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;

//Project Namespaces
using restbed::detail::SessionImpl;

//External Namespaces
using asio::buffer;
using asio::error_code;

namespace restbed
{
    Session::Session( const string& id ) : m_pimpl( new SessionImpl )
    {
        if ( id.empty( ) )
        {
            throw invalid_argument( "Session must have a unique identifier." );
        }

        m_pimpl->id = id;
    }
    
    Session::~Session( void )
    {
        delete m_pimpl;
    }

    bool Session::has( const string& name ) const
    {
        return m_pimpl->context.find( name ) not_eq m_pimpl->context.end( );
    }

    void Session::erase( const string& name )
    {
        if ( name.empty( ) )
        {
            m_pimpl->context.clear( );
        }
        else
        {
            m_pimpl->context.erase( name );
        }
    }

    const set< string > Session::keys( void ) const
    {
        std::set< std::string > keys;

        for( const auto& value : m_pimpl->context )
        {
            keys.insert( keys.end( ), value.first );
        }

        return keys;
    }
    
    bool Session::is_open( void ) const
    {
        return m_pimpl->socket not_eq nullptr and m_pimpl->socket->is_open( );
    }
    
    bool Session::is_closed( void ) const
    {
        return not is_open( );
    }
    
    void Session::close( const Bytes& body )
    {
        m_pimpl->socket->write( body, [ this ]( const asio::error_code & error, size_t )
        {
            if ( error )
            {
                const auto message = String::format( "Close failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            else
            {
                m_pimpl->close( );
            }
        } );
    }
    
    void Session::close( const Response& response )
    {
        m_pimpl->transmit( response, [ this ]( const asio::error_code & error, size_t )
        {
            if ( error )
            {
                const auto message = String::format( "Close failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            
            m_pimpl->socket->close( );
            m_pimpl->session_manager->purge( m_pimpl->session, nullptr );
        } );
    }

    void Session::close( const string& body )
    {
        close( Bytes( body.begin( ), body.end( ) ) );
    }
    
    void Session::close( const int status, const Bytes& body )
    {
        static multimap< string, string > empty;
        close( status, body, empty );
    }
    
    void Session::close( const int status, const string& body )
    {
        static multimap< string, string > empty;
        close( status, body, empty );
    }
    
    void Session::close( const int status, const multimap< string, string >& headers )
    {
        close( status, "", headers );
    }
    
    void Session::close( const int status, const string& body, const multimap< string, string >& headers )
    {
        close( status, String::to_bytes( body ), headers );
    }
    
    void Session::close( const int status, const Bytes& body, const multimap< string, string >& headers )
    {
        Response response;
        response.set_body( body );
        response.set_headers( headers );
        response.set_status_code( status );
        
        close( response );
    }
    
    void Session::yield( const Bytes& body, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        m_pimpl->socket->write( body, [ this, callback ]( const asio::error_code & error, size_t )
        {
            if ( error )
            {
                const auto message = String::format( "Yield failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            else
            {
                callback( m_pimpl->session );
            }
        } );
    }

    void Session::yield( const string& body, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        yield( String::to_bytes( body ), callback );
    }
    
    void Session::yield( const Response& response, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        m_pimpl->transmit( response, [ this, callback ]( const asio::error_code & error, size_t )
        {
            if ( error )
            {
                const auto message = String::format( "Yield failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            else
            {
                if ( callback == nullptr )
                {
                    m_pimpl->fetch( m_pimpl->session, m_pimpl->router );
                }
                else
                {
                    callback( m_pimpl->session );
                }
            }
        } );
    }

    void Session::yield( const int status, const Bytes& body, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        static multimap< string, string > empty;
        yield( status, body, empty, callback );
    }

    void Session::yield( const int status, const string& body, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        static multimap< string, string > empty;
        yield( status, body, empty, callback );
    }
    
    void Session::yield( const int status, const multimap< string, string >& headers, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        yield( status, "", headers, callback );
    }
    
    void Session::yield( const int status, const string& body, const multimap< string, string >& headers, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        yield( status, String::to_bytes( body ), headers, callback );
    }
    
    void Session::yield( const int status, const Bytes& body, const multimap< string, string >& headers, const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        Response response;
        response.set_body( body );
        response.set_headers( headers );
        response.set_status_code( status );
        
        yield( response, callback );
    }
    
    void Session::fetch( const function< void ( const shared_ptr< Session >& ) >& callback )
    {
        m_pimpl->fetch( m_pimpl->session, callback );
    }
    
    void Session::fetch( const size_t length, const function< void ( const shared_ptr< Session >&, const Bytes& ) >& callback )
    {
        if ( length > m_pimpl->buffer->size( ) )
        {
            size_t size = length - m_pimpl->buffer->size( );
            
            m_pimpl->socket->read( m_pimpl->buffer, size, [ this, length, callback ]( const asio::error_code & error, size_t )
            {
                if ( error )
                {
                    const auto message = String::format( "Fetch failed: %s", error.message( ).data( ) );
                    m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
                }
                else
                {
                    const auto data = m_pimpl->fetch_body( length );
                    callback( m_pimpl->session, data );
                }
            } );
        }
        else
        {
            const auto data = m_pimpl->fetch_body( length );
            callback( m_pimpl->session, data );
        }
    }
    
    void Session::fetch( const string& delimiter, const function< void ( const shared_ptr< Session >&, const Bytes& ) >& callback )
    {
        m_pimpl->socket->read( m_pimpl->buffer, delimiter, [ this, callback ]( const asio::error_code & error, size_t length )
        {
            if ( error )
            {
                const auto message = String::format( "Fetch failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            else
            {
                const auto data = m_pimpl->fetch_body( length );
                callback( m_pimpl->session, data );
            }
        } );
    }
    
    void Session::wait_for( const hours& delay, const function< void ( const shared_ptr< Session >& ) >& callback, const function< hours ( const hours& ) >& trigger )
    {
        if ( trigger not_eq nullptr )
        {
            const auto wrapper = [ trigger ]( const microseconds& delay )
            {
                return duration_cast< microseconds >( trigger( duration_cast< hours >( delay ) ) );
            };

            wait_for( duration_cast< microseconds >( delay ), callback, wrapper );
        }
        else
        {
            wait_for( duration_cast< microseconds >( delay ), callback, nullptr );
        }
    }
    
    void Session::wait_for( const minutes& delay, const function< void ( const shared_ptr< Session >& ) >& callback, const function< minutes ( const minutes& ) >& trigger )
    {
        if ( trigger not_eq nullptr )
        {
            const auto wrapper = [ trigger ]( const microseconds& delay )
            {
                return duration_cast< microseconds >( trigger( duration_cast< minutes >( delay ) ) );
            };

            wait_for( duration_cast< microseconds >( delay ), callback, wrapper );
        }
        else
        {
            wait_for( duration_cast< microseconds >( delay ), callback, nullptr );
        }
    }
    
    void Session::wait_for( const seconds& delay, const function< void ( const shared_ptr< Session >& ) >& callback, const function< seconds ( const seconds& ) >& trigger )
    {
        if ( trigger not_eq nullptr )
        {
            const auto wrapper = [ trigger ]( const microseconds& delay )
            {
                return duration_cast< microseconds >( trigger( duration_cast< seconds >( delay ) ) );
            };

            wait_for( duration_cast< microseconds >( delay ), callback, wrapper );
        }
        else
        {
            wait_for( duration_cast< microseconds >( delay ), callback, nullptr );
        }
    }
    
    void Session::wait_for( const milliseconds& delay, const function< void ( const shared_ptr< Session >& ) >& callback, const function< milliseconds ( const milliseconds& ) >& trigger )
    {
        if ( trigger not_eq nullptr )
        {
            const auto wrapper = [ trigger ]( const microseconds& delay )
            {
                return duration_cast< microseconds >( trigger( duration_cast< milliseconds >( delay ) ) );
            };

            wait_for( duration_cast< microseconds >( delay ), callback, wrapper );
        }
        else
        {
            wait_for( duration_cast< microseconds >( delay ), callback, nullptr );
        }
    }
    
    void Session::wait_for( const microseconds& delay, const function< void ( const shared_ptr< Session >& ) >& callback, const function< microseconds ( const microseconds& ) >& trigger )
    {
        m_pimpl->socket->wait( delay, [ delay, trigger, callback, this ]( const error_code & error )
        {
            if ( error )
            {
                const auto message = String::format( "Wait failed: %s", error.message( ).data( ) );
                m_pimpl->failure( 500, runtime_error( message ), m_pimpl->session );
            }
            else
            {
                if ( trigger == nullptr )
                {
                    callback( m_pimpl->session );
                }
                else
                {
                    const microseconds new_interval = trigger( delay );

                    if ( new_interval == microseconds::zero( ) )
                    {
                        callback( m_pimpl->session );
                    }
                    else
                    {
                        wait_for( new_interval, callback, trigger );
                    }
                }
            }
        } );
    }
    
    const string& Session::get_id( void ) const
    {
        return m_pimpl->id;
    }
    
    const string Session::get_origin( void ) const
    {
        if ( m_pimpl->socket == nullptr )
        {
            return "";
        }

        return m_pimpl->socket->get_remote_endpoint( );
    }
    
    const string Session::get_destination( void ) const
    {
        if ( m_pimpl->socket == nullptr )
        {
            return "";
        }

        return m_pimpl->socket->get_local_endpoint( );
    }
    
    const shared_ptr< const Request > Session::get_request(  void ) const
    {
        return m_pimpl->request;
    }
    
    const shared_ptr< const Resource > Session::get_resource(  void ) const
    {
        return m_pimpl->resource;
    }
    
    const multimap< string, string >& Session::get_headers( void ) const
    {
        return m_pimpl->headers;
    }

    const ContextValue& Session::get( const string& name ) const
    {
        return m_pimpl->context.at( name );
    }

    const ContextValue& Session::get( const string& name, const ContextValue& default_value ) const
    {
        if ( has( name ) )
        {
            return m_pimpl->context.at( name );
        }

        return default_value;
    }

    void Session::set( const string& name, const ContextValue& value )
    {
        m_pimpl->context.insert( make_pair( name, value ) );
    }
    
    void Session::set_header( const string& name, const string& value )
    {
        m_pimpl->headers.insert( make_pair( name, value ) );
    }
    
    void Session::set_headers( const multimap< string, string >& values )
    {
        m_pimpl->headers = values;
    }
}
