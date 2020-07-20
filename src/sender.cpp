#include "../include/sender.h"

using namespace std;

int
main() 
{
    string source_path, target_path, keep_str;
    ssize_t file_size;
    bool keep;
    struct sigaction sa;

    sa.sa_handler = int_exit;
	sigaction(SIGINT, &sa,  NULL);
    
    keep = false;
    
    cout << "Establishing connection..." << endl;
    if( setup_connection() == SUCCES )
        cout << "Connection established" << endl;
    else
    {
        error_routine();
        return FAILURE;
    }

    do
    {
        cout << "Waiting for receiver..." << endl;
        listen();
        
        cout << "Receiver founded" << endl;
        if( send_message( HANDSHAKE_MSG ) == FAILURE )
        {
            error_routine();
            return FAILURE;
        }

        cout << "File name to transfer:" << endl;
        user_input( &source_path );

        if( send_message( get_file_name( source_path, source_path.length() ) ) == FAILURE )
        {
            error_routine();
            return FAILURE;
        }
        
        file_size = get_file_size( &source_path );
        if( file_size <= 0 )
        {
            error_routine();
            return FAILURE;
        }
        else if( send_message( to_string( file_size ) ) == FAILURE )
        {
            error_routine();
            return FAILURE;
        }
        

        if( send_to_receiver( source_path ) == FAILURE )
        {
            error_routine();
            return FAILURE;
        }
        else
            cout << "File has been sended" << endl;

        while( true )
        {
            cout << endl << "Do you want to send more files?[y/n]" << endl;
            user_input( &keep_str );
            if( keep_str[0] == 'y' )
            {
                keep = true;
                break;
            }
            else if( keep_str[0] == 'n' )
            {
                keep = false;
                break;
            }
        }
        
    } while( keep );

    close_sender_connection();

    exit_routine();

    return SUCCES;
}

void
user_input( string *input_buffer )
{
    do
    {
        cout << ">> ";  
        getline( cin, *input_buffer );
    } while( input_buffer->empty() );
}

string
get_file_name( string path_to_file, size_t len )
{
    int i;
    bool slash = false;

    for( i = (int)len - 1; i >= 0; i-- )
    {
        if( path_to_file[i] == '/' )
        {
            slash = true;
            break;
        }
    }
    
    if( slash )
        return path_to_file.substr( i + 1 );
    else
        return path_to_file;
}

ssize_t
get_file_size( string *file_path )
{
    FILE* file;
    ssize_t size;

    file = fopen( file_path->c_str(), "rb" );
    size = 0;

    if( file != NULL ) 
    {
        fseek( file, 0L, SEEK_END);
        size = ftell(file);
        fclose(file);
    }

    return size;
}

void
int_exit( int sig ) 
{
	if( sig > 0 ) 
    {
		send_message( INT_MSG );
	}
    close_sender_connection();
    exit_routine();
	exit(SUCCES);
}