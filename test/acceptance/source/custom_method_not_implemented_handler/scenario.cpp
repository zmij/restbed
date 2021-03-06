/*
 * Copyright (c) 2013, 2014, 2015 Corvusoft
 */

//System Includes
#include <thread>
#include <string>
#include <memory>
#include <utility>
#include <stdexcept>
#include <functional>

//Project Includes
#include <restbed>
#include "http.hpp"

//External Includes
#include <catch.hpp>

//System Namespaces
using std::thread;
using std::string;
using std::function;
using std::make_pair;
using std::shared_ptr;
using std::make_shared;

//Project Namespaces
using namespace restbed;

//External Namespaces

void method_not_implemented_handler( const shared_ptr< Session >& session )
{
    session->close( -232, "Banned Method", { { "Content-Length", "13" } } );
}

SCENARIO( "custom resource method not implemented handler", "[resource]" )
{
    GIVEN( "I publish a resource at '/resources/1' without a HTTP 'GET' method handler" )
    {
        auto resource = make_shared< Resource >( );
        resource->set_path( "/resources/1" );

        auto settings = make_shared< Settings >( );
        settings->set_port( 1984 );
        settings->set_default_header( "Connection", "close" );

        shared_ptr< thread > worker = nullptr;

        Service service;
        service.publish( resource );
        service.set_method_not_implemented_handler( method_not_implemented_handler );
        service.set_ready_handler( [ &worker ]( Service& service )
        {
            worker = make_shared< thread >( [ &service ] ( )
            {
                WHEN( "I perform a HTTP 'PUT' request to '/resources/1'" )
                {
                    Http::Request request;
                    request.port = 1984;
                    request.host = "localhost";
                    request.path = "/resources/1";

                    auto response = Http::put( request );

                    THEN( "I should see a '-232' (Banned Method) status code" )
                    {
                        REQUIRE( -232 == response.status_code );
                    }

                    AND_THEN( "I should see a repsonse body of 'Banned Method'" )
                    {
                        Bytes expection { 'B', 'a', 'n', 'n', 'e', 'd', ' ', 'M', 'e', 't', 'h', 'o', 'd' };
                        REQUIRE( response.body == expection );
                    }

                    AND_THEN( "I should see a 'Connection' header value of 'close'" )
                    {
                        auto header = response.headers.find( "Connection" );
                        REQUIRE( header not_eq response.headers.end( ) );
                        REQUIRE( "close" == response.headers.find( "Connection" )->second );
                    }

                    AND_THEN( "I should see a 'Content-Length' header value of '13'" )
                    {
                        auto header = response.headers.find( "Content-Length" );
                        REQUIRE( header not_eq response.headers.end( ) );
                        REQUIRE( "13" == response.headers.find( "Content-Length" )->second );
                    }
                }

                service.stop( );
            } );
        } );
        service.start( settings );
        worker->join( );
    }
}
