/*
 * Copyright (c) 2013, 2014, 2015 Corvusoft
 */

#ifndef _RESTBED_SESSION_H
#define _RESTBED_SESSION_H 1

//System Includes
#include <map>
#include <set> 
#include <chrono>
#include <string>
#include <memory>
#include <functional>

//Project Includes
#include <corvusoft/restbed/byte.hpp>
#include <corvusoft/restbed/string.hpp>
#include <corvusoft/restbed/context_value.hpp>

//External Includes

//System Namespaces

//Project Namespaces

//External Namespaces

namespace restbed
{
    //Forward Declarations
    class Request;
    class Session;
    class Response;
    class Resource;
    
    namespace detail
    {
        class SessionImpl;
        class ServiceImpl;
    }
    
    class Session
    {
        public:
            //Friends
            
            //Definitions
            
            //Constructors
            explicit Session( const std::string& id );
            
            virtual ~Session( void );
            
            //Functionality
            bool has( const std::string& name ) const;

            void erase( const std::string& name = "" );

            const std::set< std::string > keys( void ) const;

            bool is_open( void ) const;
            
            bool is_closed( void ) const;
            
            void close( const Bytes& body );

            void close( const Response& response );
            
            void close( const std::string& body = "" );
            
            void close( const int status, const Bytes& body );
            
            void close( const int status, const std::string& body = "" );
            
            void close( const int status, const std::multimap< std::string, std::string >& headers );
            
            void close( const int status, const std::string& body, const std::multimap< std::string, std::string >& headers );
            
            void close( const int status, const Bytes& body, const std::multimap< std::string, std::string >& headers );
            
            void yield( const Bytes& data, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );

            void yield( const std::string& data, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );
            
            void yield( const Response& response, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );

            void yield( const int status, const std::string& body, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );
            
            void yield( const int status, const Bytes& body = { }, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );
            
            void yield( const int status, const std::multimap< std::string, std::string >& headers, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );
            
            void yield( const int status, const Bytes& body, const std::multimap< std::string, std::string >& headers, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );

            void yield( const int status, const std::string& body, const std::multimap< std::string, std::string >& headers, const std::function< void ( const std::shared_ptr< Session >& ) >& callback = nullptr );
            
            void fetch( const std::function< void ( const std::shared_ptr< Session >& ) >& callback );
            
            void fetch( const std::size_t length, const std::function< void ( const std::shared_ptr< Session >&, const Bytes& ) >& callback );
            
            void fetch( const std::string& delimiter, const std::function< void ( const std::shared_ptr< Session >&, const Bytes& ) >& callback );
            
            void wait_for( const std::chrono::hours& delay, const std::function< void ( const std::shared_ptr< Session >& ) >& callback, const std::function< std::chrono::hours ( const std::chrono::hours& ) >& trigger = nullptr );
            
            void wait_for( const std::chrono::minutes& delay, const std::function< void ( const std::shared_ptr< Session >& ) >& callback, const std::function< std::chrono::minutes ( const std::chrono::minutes& ) >& trigger = nullptr );
            
            void wait_for( const std::chrono::seconds& delay, const std::function< void ( const std::shared_ptr< Session >& ) >& callback, const std::function< std::chrono::seconds ( const std::chrono::seconds& ) >& trigger = nullptr );
            
            void wait_for( const std::chrono::milliseconds& delay, const std::function< void ( const std::shared_ptr< Session >& ) >& callback, const std::function< std::chrono::milliseconds ( const std::chrono::milliseconds& ) >& trigger = nullptr );
            
            void wait_for( const std::chrono::microseconds& delay, const std::function< void ( const std::shared_ptr< Session >& ) >& callback, const std::function< std::chrono::microseconds ( const std::chrono::microseconds& ) >& trigger = nullptr );
            
            //Getters
            const std::string& get_id( void ) const;
            
            const std::string get_origin( void ) const;
            
            const std::string get_destination( void ) const;
            
            const std::shared_ptr< const Request > get_request(  void ) const;
            
            const std::shared_ptr< const Resource > get_resource( void ) const;
            
            const std::multimap< std::string, std::string >& get_headers( void ) const;

            const ContextValue& get( const std::string& name ) const;

            const ContextValue& get( const std::string& name, const ContextValue& default_value ) const;
            
            //Setters
            void set( const std::string& name, const ContextValue& value );

            void set_header( const std::string& name, const std::string& value );
            
            void set_headers( const std::multimap< std::string, std::string >& values );
            
            //Operators
            
            //Properties
            
        protected:
            //Friends
            
            //Definitions
            
            //Constructors
            
            //Functionality
            
            //Getters
            
            //Setters
            
            //Operators
            
            //Properties
            
        private:
            //Friends
            friend detail::ServiceImpl;
            friend detail::SessionImpl;
            
            //Definitions
            
            //Constructors
            Session( void ) = delete;
            
            Session( const Session& original ) = delete;
            
            //Functionality
            
            //Getters
            
            //Setters
            
            //Operators
            Session& operator =( const Session& value ) = delete;
            
            //Properties
            detail::SessionImpl* m_pimpl;
    };
}

#endif  /* _RESTBED_SESSION_H */
