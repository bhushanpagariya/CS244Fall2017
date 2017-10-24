<?php
//var_dump($_POST); 
if($_SERVER['REQUEST_METHOD'] == 'POST') {
  $post = json_decode(file_get_contents('php://input'));

  $filename = "/var/www/html/data.txt";
  $file = fopen( $filename, "a" );

  if( $file == false ) {
          echo ( "Error in opening new file" );
          exit();
   }

  for($i=0;$i<count($post->ir);$i++){
          fwrite( $file, $post->ir[$i].",".$post->red[$i]."\n");
  }
  echo "Success!!!!!!";
  fclose( $file );
  exit();
}
?>