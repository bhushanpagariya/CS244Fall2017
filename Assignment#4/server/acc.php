<?php
//var_dump($_POST); 
if($_SERVER['REQUEST_METHOD'] == 'POST') {
        //echo $_POST;
        $post = json_decode(file_get_contents('php://input'));

        $filename = "/var/www/html/acc_reading.txt";
        $file = fopen( $filename, "a" );

        if( $file == false ) {
                echo ( "Error in opening new file" );
                exit();
         }

        for($i=0;$i<count($post->x);$i++){
                fwrite( $file, $post->x[$i].",".$post->y[$i].",".$post->z[$i]."\n");
        }
        echo "Success!!!!!!";
        fclose( $file );
        exit();
}
?>