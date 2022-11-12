package main

import (
	"fmt"
	"io"
	"log"
	"net"
)

var client_list []net.Conn

func main() {
	l, err := net.Listen("tcp", ":1129")
	if nil != err {
		log.Println(err)
	}
	defer l.Close()

	log.Println("SOCKET SERVER IS ON")

	for {
		conn, err := l.Accept()
		if nil != err {
			log.Println(err)
			continue
		}
		defer conn.Close()
		go ConnHandler(conn)
	}
}

func ConnHandler(conn net.Conn) {
	recvBuf := make([]byte, 4096)
	for {
		n, err := conn.Read(recvBuf)
		if nil != err {
			if io.EOF == err {
				log.Println(err)
				return
			}
			log.Println(err)
			return
		}
		if 0 < n {
			data := recvBuf[:n]
			log_str := fmt.Sprintf("[%s]: %s", conn.RemoteAddr(), string(data))
			log.Println(log_str)
			_, err = conn.Write(data[:n])
			if err != nil {
				log.Println(err)
				return
			}

			for _, c := range client_list {
				log.Println(c.RemoteAddr())
				if c != conn {
					// advertiser
					_, err = c.Write(data[:n])
					if err != nil {
						log.Println(err)
					}
				}
			}
		}
	}
}
