const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const mongoose = require("mongoose");

const app = express();

app.set("view engine", "ejs");
app.use(express.static("public"));
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
app.use(cors())

// CONNECT TO DB
const uri = "mongodb://127.0.0.1:27017/objects";
mongoose.set('strictQuery', false);
mongoose.connect(uri, (err) => {
    if (err) {
        console.log(err);
    } else {
        console.log("Connected to database");
    }
});

// CREATE DATABASE SCHEMA
const objectSchema = {
    message: String,
    cm: Number,
    inches: Number
};

const Object = mongoose.model("Object", objectSchema);


// POST AND GET
app.post("/object", (req, res) => {

    const newObject = new Object({
        message: req.body.message,
        cm:  req.body.cm,
        inches:  req.body.inches
    });

    newObject.save((err, result) => {
        if (err) {
            console.log(err);
        } else {
            console.log("Successfully add new object to DB");
        }
    });

});

app.get("/object", (req, res) => {

    Object.find({}, (err, result) => {

        if (err) {
            console.log(err);
        } else {
            res.send(result);
        }

    })

});




// SERVER LISTENING
app.listen("3000", () => {
    console.log("Server started on port 3000");
});