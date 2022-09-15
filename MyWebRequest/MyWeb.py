from http.server import BaseHTTPRequestHandler, HTTPServer
import time
hostName = "localhost"
serverPort = 8000

class MyServer(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head></html>","utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("<body><p>This is an example web server.</p></body>","utf-8"))

    def do_POST(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        if self.path == '/':
            self.wfile.write(bytes('{"status":"ng"}',"utf-8"))
        else:
            try:
                content_length = int(self.headers['Content-Length'])
                post_data = self.rfile.read(content_length)
                print(post_data)
                self.wfile.write(bytes('{"status":"ok"}',"utf-8"))
                with open("." + self.path,"w") as f:
                    f.write(post_data.decode())
                    f.close()
            except BaseException:
                pass

webServer = HTTPServer((hostName, serverPort),MyServer)
print("Server started http://%s:%s" % (hostName, serverPort))

try:
    webServer.serve_forever()
except KeyboardInterrupt:
    pass

webServer.server_close()
print("Server stopped.")
