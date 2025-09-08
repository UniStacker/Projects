import os
import pytesseract
from pdf2image import convert_from_path

# Set the path where your PDF files are stored
pdf_folder = input("Enter PDF Folder Path: ")
output_folder = input("Enter Output Folder Path: ")

# Ensure the output folder exists
os.makedirs(output_folder, exist_ok=True)

# Function to extract text from PDFs using OCR
def extract_text_with_ocr(folder_path):
    for file in sorted(os.listdir(folder_path)):
        if file.lower().endswith(".pdf"):
            file_path = os.path.join(folder_path, file)
            print(f"Processing: {file}")

            # Convert PDF pages to images
            images = convert_from_path(file_path)

            # Extract text from images using OCR
            extracted_text = "\n".join([pytesseract.image_to_string(image) for image in images])

            # Save extracted text to a file
            text_filename = os.path.join(output_folder, f"{os.path.splitext(file)[0]}.txt")
            with open(text_filename, "w", encoding="utf-8") as text_file:
                text_file.write(extracted_text)

            print(f"Saved extracted text: {text_filename}")

# Run the OCR extraction
extract_text_with_ocr(pdf_folder)
