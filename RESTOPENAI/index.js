const OpenAI = require('openai')
const express = require('express');
const app = express();
const port = 3000;
const util = require("util");
const fs = require('fs')
const textToSpeech = require('@google-cloud/text-to-speech');
const client = new textToSpeech.TextToSpeechClient();
const openai = new OpenAI({
    apiKey: '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',
});
const GoogleTts = require("google-tts.js") 

const delayMiddleware = (req, res, next) => {
  setTimeout(next, 5000);
};
app.get('/openai',delayMiddleware, async (req, res) => {
  const query = req.query.q || 'Hello';
  const chatcompletions = await openai.chat.completions.create({
    messages: [{ role: "user", content: `${query}` }],
    model: "gpt-3.5-turbo",
});
  res.json({data: `${chatcompletions.choices[0].message.content}`});
});
app.get ("/asmaulh", async (res) => {
  const apps = fs.readFileSync("./lib/AsmaulHusna.json", "utf-8");
  const parseApps = JSON.parse(apps);
  res.json({data : parseApps});
})
app.get("/texttospeech", async (req,res) =>{
 const query = req.query.q || "Hello"
 const response = await GoogleTts.getUrl(`${query}`, "id")
 res.json(response);
})

app.listen(port, () => {
  console.log(`Server is running at http://localhost:${port}`);
});