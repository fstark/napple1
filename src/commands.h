int executeCommandString( const char *command );
int executeCommandFile( const char *filename );
int executeCommand( int argc, const char **argv );

// maye this should be in keyboard.[hc]?
int isBound( int key );
const char *getBoundCommand( int key );
