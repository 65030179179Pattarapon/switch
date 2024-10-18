const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const mongoose = require('mongoose');

const app = express();
const PORT = 3305;

// API Key
const API_KEY = 'pgGWvY38jR0khFKAvCV1640cJzzqyQIjWfswLrrLFYYNWOSulMrC1Hl3TroeMRyCcVNdWFh3Rs3Nr1PNMbARr3d1EC1tuW3kf1NYbijuentmM7rGrr3jBc9JKf1S5Dhn'; // สร้าง API key ของคุณเองที่นี่

// Middleware
app.use(cors());  // Enable CORS for all routes
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Connect to MongoDB
mongoose.connect('mongodb://mongodb:27017/sensorDB', {
    useNewUrlParser: true,
    useUnifiedTopology: true,
})
.then(() => console.log('Connected to MongoDB'))
.catch(err => console.error('Could not connect to MongoDB:', err));

// Define a schema for the button data
const buttonSchema = new mongoose.Schema({
    count: { type: Number, required: true },
    timestamp: { type: Date, default: Date.now }
});

// Create a model from the schema
const ButtonData = mongoose.model('ButtonData', buttonSchema);

// Store the last received button data
let lastButtonData = {};

// Endpoint for handling incoming button data
app.post('/sensor-data', (req, res) => {
    const clientApiKey = req.headers['x-api-key']; // ดึง API key จาก headers

    // ตรวจสอบ API key
    if (clientApiKey !== API_KEY) {
        return res.status(403).send('Forbidden: Invalid API Key');
    }

    lastButtonData = {
        count: req.body.count,  // Receive count from the client
        timestamp: new Date()   // Update timestamp to the current time
    };
    console.log(lastButtonData);  // Log incoming data

    // Create a new button data document
    const buttonData = new ButtonData(lastButtonData);

    // Save the document to the database
    buttonData.save()
        .then(() => {
            console.log('Data saved:', lastButtonData);
            res.send('Data received and saved to MongoDB');
        })
        .catch(error => {
            console.error('Error saving data:', error);
            res.status(500).send('Error saving data to MongoDB');
        });
});

// Endpoint to get the last button data
app.get('/sensor-data', (req, res) => {
    res.json(lastButtonData);  // Return the last button data as JSON
});

// Serve static files (e.g., the front-end)
app.use(express.static('public'));

// Start the server
app.listen(PORT, '0.0.0.0', () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});
