import { useEffect, useState } from "react";
import "./App.css";

export default function App() {
  const [startTime, setStartTime] = useState("");
  const [endTime, setEndTime] = useState("");
  const [saved, setSaved] = useState(false);


  useEffect(() => {
    // Default-Werte vom Backend laden
    async function fetchBoolean() {
      try {
        const res = await fetch("http://vm-l-vps03.leon-malte.de:3000/getBoolean");
        const data = await res.json();
      } catch (err) {
        console.error("Fehler beim Laden:", err);
      }
    }
    async function fetchSchedule() {
      try {
        const res = await fetch("http://vm-l-vps03.leon-malte.de:3000/getSchedule");
        const data = await res.json();

        setStartTime(data.start_time);
        setEndTime(data.end_time);
      } catch (err) {
        console.error("Fehler beim Laden:", err);
      }
    }
    fetchBoolean();
    fetchSchedule();
  }, []);

  async function handleSubmit(e) {
    e.preventDefault();

    try {
      const res = await fetch("http://vm-l-vps03.leon-malte.de:3000/setSchedule", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          startTime,
          endTime,
        }),
      });

      if (res.ok) {
        setSaved(true);
        setTimeout(() => setSaved(false), 800);
      }
    } catch (err) {
      console.error("Speichern fehlgeschlagen:", err);
    }
  }

  return (
    <div className="container">
      <form className="card" onSubmit={handleSubmit}>
        <div className="field">
          <h2>Startzeit</h2>
          <input
            type="time"
            value={startTime}
            onChange={(e) => setStartTime(e.target.value)}
          />
        </div>

        <div className="field">
          <h2>Endzeit</h2>
          <input
            type="time"
            value={endTime}
            onChange={(e) => setEndTime(e.target.value)}
          />
        </div>

        <button
          type="submit"
          className={saved ? "flash" : ""}
        >
          {saved ? "Gespeichert âœ“" : "Festlegen"}
        </button>

      </form>
    </div>
  );
}