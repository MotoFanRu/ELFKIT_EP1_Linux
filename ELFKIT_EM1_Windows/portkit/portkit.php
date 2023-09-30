<?php
/*
    Copyright (c) 2008 flash.tato & theCor3

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

if( $argc <= 1 )
{
    echo "# ElfLoader port tool for V3x/E770/V1075/E1070\n";
    echo "# by flash.tato & theCor3\n";
    echo "Help:\n";
    echo "\t-sh: build library.def from .h files <-sh> (do not use)\n";
    echo "\t-f:  build functions.pat <-f library.def functions.pat CG1.smg> (do not use)\n";
    echo "\t-sf: build library.def from functions.pat <-sf functions.pat CG1.smg>\n";
    echo "\t-fl: fixes ldr.bin headers with new addresses <-fl new_library.def>\n";
    echo "\t-install: to install the elf loader spwner <-install cg1.smg address_of_spwner>\n";
}

$already_in = array( );
$_functions_db = array( );

if( $argv[ 1 ] == '-sh' )
{
    $h = opendir( "./" );

    $output = "";

    $houtput = fopen( "./library.def", "w+" );

    while( $file = readdir( $h ) )
    {
        if( $file == '.' ||
            $file == '..' ||
            substr( $file, strlen( $file ) - 2, 2 ) != '.h' ||
            is_dir( $file ) ) continue;
            
        $file_content = file_get_contents( "./" . $file );

        preg_match_all("#typedef\s*([a-zA-Z0-9_* ]+)\s*\(\s*\*\s*([a-zA-Z0-9_]+)\s*\)\s*\((.*?)\);\r?\n?.*?0x([A-Fa-f0-9]{8});#s", $file_content, $retn, PREG_SET_ORDER );

        foreach( $retn as $c )
        {
            if( !in_array( str_replace( "f_", "", $c[ 2 ] ), $already_in ) )
            {
                $output .= str_replace( "f_", "", $c[ 2 ] ) . "=" . $c[ 4 ] . "\n";
                $already_in[ ] = str_replace( "f_", "", $c[ 2 ] );
            }
        }
    }

    fwrite( $houtput, $output );
    fclose( $houtput );
}
elseif( $argv[ 1 ] == '-install' )
{
    $input = file_get_contents( "./" . $argv[ 2 ] );

    $GPSNI_Register = "24709F00????????????????60136025????????60048F00666312452070";
    
    $address = find_bytes( $input, $GPSNI_Register, 0 ) + hexdec( "10040000" );

    $a = pack( "H*", $argv[ 3 ] );
    $b = pack( "H*", dechex( $address ) );
    
    if( strpos( $input, $b, 0 ) === FALSE )
        die( 'Cannot found GPSNI_Register this means i can\'t install the elfloader!\n' );

    $input = str_replace( $b, $a, $input );

    $oh = fopen( "./" . $argv[ 2 ], "w+" );
    fwrite( $oh, $input );
    fclose( $oh );
}
elseif( $argv[ 1 ] == '-fl' )
{
    $output = "";

    $nlib = file_get_contents( "./" . $argv[ 2 ] );

    preg_match_all("#([a-zA-Z0-9_]+)\s*\=\s*([A-Fa-f0-9]*)#s", $nlib, $ndefs, PREG_SET_ORDER );

    for( $i = 0; $i < count( $ndefs ); $i++ )
    {
        $output .= "\t.equ " . $ndefs[ $i ][ 1 ] . ", 0x" . $ndefs[ $i ][ 2 ] . "\n";
        $output .= "\t.export " . $ndefs[ $i ][ 1 ] . "\n";
    }
    
    $output .= "\n";
    
    $oh = fopen( "./functions.asm", "w+" );
    fwrite( $oh, $output );
    fclose( $oh );
}
elseif( $argv[ 1 ] == '-f' )
{
    $houtput = fopen( $argv[ 3 ], "w+" );
    
    $lib = file_get_contents( "./" . $argv[ 2 ] );
    $input = file_get_contents( "./" . $argv[ 4 ] );

    $functions = explode( "\n", $lib );
    
    foreach( $functions as $function )
    {
        $data = explode( "=", $function );
        $address = hexdec( $data[ 1 ] ) - hexdec( "10040000" );

        $code = "";
        for( $i = 0; $i < 17; $i++ )
            $code .= $input[ $address + $i ];

        $hx_code = "";
        
        echo $data[ 0 ] . " at 0x" . dechex( $address ) + hexdec( "10040000" ) . "\n";
        $dt = unpack( "C*", $code );
        
        for( $i = 1; $i < count( $dt ); $i++ )
        {
            $hx_code .= sprintf( "%02X", $dt[ $i ] );
        }

        $towrite = $data[ 0 ] . "=" . $hx_code;
        fwrite( $houtput, $towrite . "\n" );
    }

    fclose( $houtput );
}
elseif( $argv[ 1 ] == '-sf' )
{
    $lib = file_get_contents( "./" . $argv[ 2 ] );
    $input = file_get_contents( "./" . $argv[ 3 ] );
    $newlib = fopen( "./newlib.sym", "w+" );

    $functions = explode( "\n", $lib );
    
    foreach( $functions as $function )
    {
        $binary = "";
        $offset = 0;
        $data = explode( "=", $function );

        $binary = trim( $data[ 1 ] );

        $data[ 0 ] = rtrim( $data[ 0 ] );
        
        if( strlen( $data[ 0 ] ) <= 0 ) continue;
        
        $addr_add = 0;
        if( strpos( $data[ 0 ], "+" ) )
        {
            $_exp = explode( "+", $data[ 0 ] );
            $data[ 0 ] = trim( $_exp[ 1 ] );
            $_exp[ 0 ] = str_replace( "0x", "", $_exp[ 0 ] );
            $addr_add = hexdec( $_exp[ 0 ] );
        }
        
        if( strpos( $data[ 0 ], "," ) ) //to select the occurrence
        {
            $p = explode( ",", $data[ 0 ] );
            for( $i = 1; $i < trim( $p[ 1 ] ); $i++ )
            {
                $offset = find_bytes( $input, $binary, $offset ) + 4;
            }
            $data[ 0 ] = rtrim( $p[ 0 ] );
        }

        $faddr = find_bytes( $input, $binary, $offset );
        
        if( $faddr > 1 )
        {
            printf( "%s at %X\n", $data[ 0 ], $faddr + hexdec( "10040000" ) + $addr_add );
            $_functions_db[ $data[ 0 ] ] = $faddr + $addr_add;
            fwrite( $newlib, sprintf( "%s = %x\n", $data[ 0 ], $faddr + hexdec( "10040000" ) + $addr_add ) );
        }
        else
        {
            printf( "%s NOT Found!\n", $data[ 0 ] );
            fwrite( $newlib, sprintf( "%s=NOT FOUND SEARCH IT MANUALLY\n", $data[ 0 ] ) );
        }
    }
    
    fclose( $newlib );
}

function function_offset( $function )
{
    global $_functions_db;
    if( array_key_exists( $function, $_functions_db ) )
        return $_functions_db[ $function ];
    return -1;
}

function find_bytes( $content, $bytes, $offset = 0 )
{
	if(preg_match("#^[0-9A-Fa-f]*$#", $bytes))
	{
		$a = pack("H*", $bytes);
		return strpos($content, $a, $offset);
	}
	elseif( preg_match( "#^from\s*\(\s*([a-zA-Z0-9_]+)\s*\+\s*0x([A-Fa-f0-9]*)\)\,\s*([A-Fa-f0-9\?]*)#s", $bytes, $r ) )
	{
        $t_offset = function_offset( $r[ 1 ] ) + hexdec( $r[ 2 ] );
        if( $t_offset > 0 )
            return find_bytes( $content, $r[ 3 ], $t_offset );
	}
	else
	{
		$to_find = "#" . preg_replace("#([0-9A-Fa-f]{2})#", "\\\\x$1", $bytes) . "#";
		$to_find = str_replace("??", ".", $to_find);
		preg_match($to_find, $content, $t, PREG_OFFSET_CAPTURE, $offset);
		return $t[ 0 ][ 1 ];
	}
}

?>
