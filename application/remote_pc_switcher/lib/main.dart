import 'dart:io';

import 'package:flutter/material.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'PC Switcher',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const MyHomePage(title: 'PC Switcher'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({Key? key, required this.title}) : super(key: key);

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  late Socket tcp_socket;
  bool socket_state = false;
  bool pc_state = false;
  var response = "";

  TextEditingController controller = TextEditingController();

  void connectServer() async {
    for (var data in await NetworkInterface.list()) {
      print(data);
    }
    tcp_socket = await Socket.connect('54.180.0.192', 1129,
        timeout: Duration(seconds: 8));
    setState(() {
      socket_state = true;
    });

    print("connected to server");

    tcp_socket.listen(
      (onData) {
        print(String.fromCharCodes(onData).trim());
        setState(() {
          print(String.fromCharCodes(onData).trim());
          setState(() {
            response = (String.fromCharCodes(onData).trim());
          });
        });
      },
      onDone: () {
        disconnectServer();
        setState(() {
          socket_state = false;
        });
      },
      onError: (error) {
        print(error);
        disconnectServer();
        setState(() {
          socket_state = false;
        });
      },
    );
  }

  void disconnectServer() {
    tcp_socket.close();
    setState(() {
      socket_state = false;
    });
  }

  void send(String msg) {
    tcp_socket.write(msg);
  }

  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    disconnectServer();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                ElevatedButton(
                    onPressed: socket_state ? disconnectServer : connectServer,
                    child: Text(socket_state ? "Disconnect" : "Connect"))
              ],
            ),
            Switch(
              value: pc_state,
              onChanged: (value) {
                if (!socket_state) {
                  connectServer();
                  print("connectServer");
                }
                if (socket_state & !pc_state) {
                  setState(() {
                    pc_state = true;
                  });

                  send("PC_SW_ON");
                  print("PC_SW_ON");
                }
              },
            ),
          ],
        ),
      ),
    );
  }
}
