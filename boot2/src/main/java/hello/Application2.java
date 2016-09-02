package hello;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.CookieValue;
import org.springframework.web.bind.annotation.RequestHeader;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.net.Inet4Address;
import java.net.UnknownHostException;

@SpringBootApplication
@RestController
public class Application2
{
	private static final Logger logger = LoggerFactory.getLogger( Application2.class );

	@RequestMapping( "/login" )
	public String login()
	{
		return String.format( "Welcome to %s's login page", Application2.class.getName() );
	}

	@RequestMapping( "/" )
	public String home(
		@CookieValue( "foo" ) String foo,
		@RequestHeader( "bar" ) String bar,
		@RequestHeader( "Authorization" ) String authorization
	) throws UnknownHostException
	{
		StringBuilder sb = new StringBuilder();
		sb.append( Application2.class.getName() )
			.append( " " )
			.append( "from ip " )
			.append( Inet4Address.getLocalHost().getHostAddress() )
			.append( " " )
			.append( "fooCookie =" )
			.append( " " )
			.append( foo )
			.append( " " )
			.append( "barHeader =" )
			.append( " " )
			.append( bar )
			.append( " " )
			.append( "Authorization =" )
			.append( " " )
			.append( authorization );
		System.out.println( sb.toString() );
		logger.info( sb.toString() );
		return sb.toString();
	}

	public static void main( String[] args )
	{
		SpringApplication.run( Application2.class, args );
	}
}
