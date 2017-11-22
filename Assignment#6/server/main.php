<?php
if($_SERVER['REQUEST_METHOD'] == 'POST') {
  $post = file_get_contents('php://input');
  echo(strlen($post));
  $filename = "/var/www/html/data.txt";
  $file = fopen( $filename, "a" );
  if( $file == false ) {
          echo ( "Error in opening new file" );
          exit();
   }
  $arr = explode(",", $post);
  for($i=0;$i<count($arr)-1;$i+=5){
          fwrite( $file, $arr[$i].",".$arr[$i+1].",".$arr[$i+2].",".$arr[$i+3].",".$arr[$i+4]."\n");
  }
  echo "Success!!!!!!";
  fclose( $file );
  exit();
}
?>