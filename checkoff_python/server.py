from checkoff_python import create_app

app = create_app('config.py')

if __name__ == "__main__":
    # start Flask development server within container
    app.run(host='0.0.0.0', port=3000)
