# Restbed 

----------

> It's akin to embedding NGINX into your companies own product line.
>                                                       -- Solutions Architect, Bellrock Technology

## Features

 - Long Polling
 - Streaming
 - Comet
 - Secure Socket Layer (SSL/TLS)
 - Path Parameters
 - Query Parameters
 - Request filters
 - Custom Logging
 - Multi-path resources
 - Customisable HTTP methods
 - Compression GZip, Deflate, etc...
 - IPv4/IPv6 Network Support
 - Asynchronous single threaded architecture
 - Built-in Path, Query, and Header conversions
 - Service & Resource level authentication
 - Service & Resource level error handling
 - Secure, Stable, and extensively tested over 3+ years
 - Flexibility to address HTTP 1.0/1.1+ compliance
 - Active, vibrant community

## Example

```C++
#include <memory>
#include <cstdlib>
#include <restbed>

using namespace std;
using namespace restbed;

void get_method_handler( const shared_ptr< Session >& session )
{
    const auto request = session->get_request( );

    size_t content_length = 0;
    request->get_header( "Content-Length", content_length );

    session->fetch( content_length, [ ]( const shared_ptr< Session >& session,
                                         const Bytes& body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );

        session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );
    } );
}

int main( const int, const char** )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_method_handler );

    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_default_header( "Connection", "close" );

    Service service;
    service.publish( resource );
    service.start( settings );
    
    return EXIT_SUCCESS;
}
```

More in-depth examples can be found [here](https://github.com/Corvusoft/restbed/tree/master/example).

## License

Copyright (c) 2015 Corvusoft Limited, United Kingdom. All rights reserved. 

The Restbed Framework is dual licensed; See [LICENSE](LICENSE) for full details.

## Support

Please contact sales@corvusoft.co.uk, for support options including bespoke software development, design consultation, training, mentoring and code review.

| Production Install Base   |        Tickets      |    Anual Fee Per Install (excl. VAT)    |
| :-----------------------: |:-------------------:|:---------------------------------------:| 
|            1-3            |          3          |                £280.00                  | 
|            4-10           |          3          |                £360.00                  |
|             11+           |          3+         |                 AABUS                   |                

Purchase by contacting sales@corvusoft.co.uk for further information.

## Build


```bash
git clone --recursive https://github.com/corvusoft/restbed.git
mkdir restbed/build
cd restbed/build
cmake [-DBUILD_TESTS=YES] [-DBUILD_EXAMPLES=YES] [-DBUILD_SSL=NO] [-DCMAKE_INSTALL_PREFIX=/output-directory] ..
make [-j CPU_CORES+1] install
make test
```

You will now find all required components installed in the restbed distribution folder.  Please submit all enhancements, proposals, and defects via the [issue](http://github.com/corvusoft/restbed/issues) tracker; Alternatively ask a question on [StackOverflow](http://stackoverflow.com/) tagged with #restbed.

## Minimum Requirements

|     Resource   |                   Requirement                   |
|:--------------:|:-----------------------------------------------:| 
|       CPU      |                    40mhz                        |
|       RAM      |                     3mb                         |
|       Disk     |                     1mb                         |
|     Compiler   |          C++11 compliant or above               |
|        OS      | BSD, Linux, Mac OSX, Solaris, Windows, Raspbian |

## Road Map

|   Milestone   |                   Feature                       |      Status     |
|:-------------:|:-----------------------------------------------:|:---------------:| 
|     0.0       |         Asynchrounous HTTP Service              |     complete    |
|     1.0       |             HTTP 1.0 Compliance                 |     complete    |
|     2.0       |             HTTP 1.1 Compliance                 |     complete    |
|     2.5       |             Secure Socket Layer                 |     complete    |
|     2.5       |     Simultaneous Network Ports (HTTP/HTTPS)     |     complete    |
|     3.0       |                 Rules Engine                    |  in-development |
|     3.5       |             Session Management                  |     pending     |
|     3.6       |               Resource Caching                  |     pending     |
|     4.0       |                 Web Sockets                     |     pending     |
|     5.0       |             HTTP 2.0 compliance                 |     pending     |
|     x.x       |         Bind Service to specific NIC            |     pending     |
|     x.x       |         Client-side SSL certificates            |     pending     |
|     x.x       |      Multi-Threaded service capability          |     pending     |

## Contact

|     Method   |                   Description                   |
|:--------------|:-----------------------------------------------| 
| [Glitter](https://gitter.im/Corvusoft/restbed?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)     | Online Development/Support Chat Forum                      |
| [Twitter](http://www.twitter.com/corvusoft)                  | Tweet us your questions & feature requests.   |
| [Google+](https://plus.google.com/+CorvusoftCoUkSolutions)   | Join the community social circle.             |
| [Pledgie](https://pledgie.com/campaigns/29788)               | Lend your support via our Pledgie campaign.   |
| support@corvusoft.co.uk                                      | Support related queries.                      |
| sales@corvusoft.co.uk                                        | Sale related queries.                         |
