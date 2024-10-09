int executeCommandString( const char *command );
int executeCommandFile( const char *filename );

// maye this should be in keyboard.[hc]?
int isBound( int key );
const char *getBoundCommand( int key );
