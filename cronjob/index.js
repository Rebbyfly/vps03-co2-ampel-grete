const cron = require("node-cron");

const BASE_URL = process.env.API_URL || "http://vm-l-vps03.leon-malte.de:3000";


function buildTodayDateFromHHMM(timeStr) {
  const [hours, minutes] = timeStr.split(":").map(Number);
  const now = new Date();
  return new Date(
    now.getFullYear(),
    now.getMonth(),
    now.getDate(),
    hours,
    minutes,
    0,
    0
  );
}

async function schedulerTask() {
  try {
    const now = new Date();

    const scheduleRes = await fetch(`${BASE_URL}/getSchedule`);
    const scheduleData = await scheduleRes.json();

    const { start_time, end_time } = scheduleData;

    const startDate = buildTodayDateFromHHMM(start_time);
    const endDate = buildTodayDateFromHHMM(end_time);

    const boolRes = await fetch(`${BASE_URL}/getBoolean`);
    const currentBoolean = await boolRes.json();

    // ---------- START ----------
    if (now >= startDate) {
      if (currentBoolean === false) {
        await fetch(`${BASE_URL}/toggleBoolean`, {
          method: "POST"
        });
        console.log("Startzeit überschritten → eingeschaltet");
      }
    }

    // ---------- END ----------
    if (now >= endDate) {
      if (currentBoolean === true) {
        await fetch(`${BASE_URL}/toggleBoolean`, {
          method: "POST"
        });
        console.log("Endzeit überschritten → ausgeschaltet");
      }
    }

  } catch (err) {
    console.error("Scheduler Fehler:", err.message);
  }
}

// jede Minute
cron.schedule("* * * * *", schedulerTask);

console.log("Cron Scheduler gestartet");
