<?php
   if( $_GET["name"] || $_GET["age"] ) {
      $filename = "/var/www/html/newfile.txt";
      $file = fopen( $filename, "a" );

        if( $file == false ) {
                echo ( "Error in opening new file" );
                exit();
         }
        fwrite( $file, $_GET['name'].",".$_GET['age']."\n");
        echo "Success!!!!!!";
        fclose( $file );
        exit();
   }
?>
