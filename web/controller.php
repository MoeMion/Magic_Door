<?php
    $ip = "ADDRESS TO YOUR ESP8266"; //EXAMPLE 127.0.0.1:80
    $action = $_REQUEST["action"];
    switch ($action){
        case "open" : opendoor($ip);break;
        case "check" : check($ip);break;
        case "restart" : restart($ip);break;
        default : exit("参数非法或不存在！");
    }
    
    function opendoor($ip){
    $ch = curl_init(); //file_get_contents()在这里效率莫名的低...
    curl_setopt($ch, CURLOPT_URL, "http://$ip/open");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT , 5);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    $result = curl_exec($ch);
    curl_close($ch);
    if($result == "1"){
        exit("100");
    }elseif ($result == "2") {
        exit("200");
    }else{
        echo $result;
        exit("0");
        }
    }

    function check($ip){
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, "http://$ip/check");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT , 5);
        curl_setopt($ch, CURLOPT_TIMEOUT, 10);
        $result = curl_exec($ch);
        curl_close($ch);
        if($result == "1"){
            exit("100");
        }else{
            echo $result;
            exit("0");
        }
    }
    function restart($ip){
    $ch = curl_init(); //file_get_contents()在这里效率莫名的低...
    curl_setopt($ch, CURLOPT_URL, "http://$ip/restart");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT , 5);
    curl_setopt($ch, CURLOPT_TIMEOUT, 10);
    $result = curl_exec($ch);
    curl_close($ch);
    if($result == "1"){
        exit("100");
    }else{
        echo $result;
        exit("0");
        }
    }
?>