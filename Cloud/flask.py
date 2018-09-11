#!/usr/bin/env python

from flask import Flask, json, request, Response
app = Flask(__name__)

@app.route('/', methods=['POST'])
def get_data():
    print('Recieved from client')
    req_data = request.form
    with open("dataprac.json","a") as f: #in write mode
        f.write(str(req_data))
        f.write("\n")
    return 'test'

def hello_world():
	return 'Hello, World!'

if __name__ == "__main__":
   app.run(host="0.0.0.0")
