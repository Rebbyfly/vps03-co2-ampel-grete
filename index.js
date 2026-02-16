const express = require("express");
const { Client } = require("pg");
require('dotenv').config();

const app = express();

const client = new Client({
  host: process.env.host,
  user: process.env.user,
  password: process.env.password,
  database: process.env.database,
  port: process.env.port,
});

client.connect();

app.get("/getBoolean", async (req, res) => {
  const result = await client.query("SELECT powered FROM public.co2_ampel_grete WHERE id = 1;");
  res.json(result.rows);
});

app.post("/toggleBoolean", async (req, res) => {
  const result = await client.query("UPDATE public.co2_ampel_grete SET powered = NOT powered WHERE id = 1;");
  res.json(result.rows);
});

app.listen(3000, '0.0.0.0', () => console.log("Server läuft"));
