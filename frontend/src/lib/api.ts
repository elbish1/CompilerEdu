import axios from "axios";

// Create an Axios instance configured for our FastAPI backend
const api = axios.create({
  baseURL: "http://localhost:8001/api",
  timeout: 10000,
  headers: {
    "Content-Type": "application/json",
  },
});

export default api;
