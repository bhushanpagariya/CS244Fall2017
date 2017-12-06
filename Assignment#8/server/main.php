<?php
if($_SERVER['REQUEST_METHOD'] == 'POST') {
  $post = file_get_contents('php://input');
  $filename = "/var/www/html/data.txt";
  $file = fopen( $filename, "a" );
  if( $file == false ) {
          echo ( "Error in opening new file" );
          exit();
   }
  $arr = explode(",", $post);
  for($i=0;$i<count($arr)-1;$i+=6){
          fwrite( $file, $arr[$i].",".$arr[$i+1].",".$arr[$i+2].",".$arr[$i+3].",".$arr[$i+4].",".$arr[$i+5]."\n");
  }
  fclose( $file );
  exit();
}
?>