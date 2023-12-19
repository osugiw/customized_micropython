import ujson

def myFile():
    """Ini adalah Fungsi operasi myFile dengan try except error handling"""
    try:
        """Ini adalah operasi untuk mencari file dan membaca file"""
        with open("data.txt", "rt") as file:
            data = file.read()
            print(data)
    except Exception:
        """Jika File tidak di temukan maka akan membuat file "data.json" dengan kata 'Hello World'"""
        with open("data.txt", "wt") as file:
            file.write("Hello World\n")
            
def myFileJson():
    """Ini adalah Fungsi operasi myFile dengan try except error handling"""
    try:
        """Ini adalah operasi untuk mencari file dan membaca file"""
        with open("data.json", "rt") as file:
            data = file.read()
            dataJson = ujson.loads(data)
            print(dataJson)
            print(dataJson["voltage"])
            
    except Exception:
        """Jika File tidak di temukan maka akan membuat file "data.json" dengan json object (dataJson) """
        with open("data.json", "wt") as file:
            dataJson = ujson.loads("""{"voltage":12.05}""")
            file.write(str(ujson.dumps(dataJson)))