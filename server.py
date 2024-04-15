from flask import Flask, request, jsonify
from PIL import Image
import numpy as np
import os

app = Flask(__name__)

def image_to_matrix(image_path):
    img = Image.open(image_path).convert('L')  # Convert to grayscale
    img_array = np.array(img)
    threshold = 128  # Adjust this threshold as needed
    binary_matrix = (img_array > threshold).astype(int)
    return binary_matrix.tolist()

@app.route('/upload', methods=['POST'])
def upload_image():
    if 'image' not in request.files:
        return jsonify({'error': 'No file part'})
    image_file = request.files['image']
    if image_file.filename == '':
        return jsonify({'error': 'No selected file'})

    if image_file:
        print(os.getcwd())
        image_path = os.getcwd() + "/uploaded_image.jpg"  # Save the uploaded image
        image_file.save(image_path)
        return jsonify({'message': 'Image uploaded successfully'})

@app.route('/convert', methods=['GET'])
def convert_image_to_matrix():
    try:
        image_path = "uploaded_image.jpg"  # Path to the uploaded image
        matrix = image_to_matrix(image_path)
        return jsonify({'matrix': matrix})
    except Exception as e:
        return jsonify({'error': str(e)})

@app.route('/convert2', methods=['GET'])
def convert_image_to_matrix2():
    try:
        image_path = "uploaded_image.jpg"  # Path to the uploaded image
        matrix = image_to_matrix(image_path)
        return jsonify({'matrix': [[1,1,1,1,1,1,1,1,1,1],[1,1,1,0,0,1,1,1,1,1],[0,1,0,1,1,1,0,0,1,1],[1,1,0,0,0,0,1,1,1,1]]})
    except Exception as e:
        return jsonify({'error': str(e)})

if __name__ == '__main__':
    #app.run(debug=True)
    app.run(host='0.0.0.0')
