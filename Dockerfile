# Use Python 3.14 slim image
FROM python:3.13-slim

# Install g++ for compiling the C++ engine
RUN apt-get update && apt-get install -y g++ && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy backend requirements and install them
COPY backend/requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy the entire project
COPY . .

# Ensure the source directory exists and contains the necessary files
# The backend lifespan will automatically compile the engine if not found.

# Expose backend port
EXPOSE 8000

# Run the FastAPI server
CMD ["uvicorn", "backend.main:app", "--host", "0.0.0.0", "--port", "8000"]
