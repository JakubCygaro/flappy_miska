const API_ADDRESS_PORT = "http://127.0.0.1:8080"
const MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR = "miskaPersonalBest"
const LEADERBOARD_FETCH_AMOUNT = 15

var upload_error = null;
var upload_success = null;
var username = null;
var validation_result = null;

const registerGameImpl = async () => {
    const response = await fetch(API_ADDRESS_PORT + "/register_game", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json",
        },
    })
    if (response.ok) {
        console.log("got session cookie")
    } else {
        console.error("failed to register game start")
    }
}

const registerGameEndImpl = async () => {
    const response = await fetch(API_ADDRESS_PORT + "/register_end", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json",
        },
    })
    if (response.ok) {
        console.log("registered game end")
    } else {
        console.error("failed to register game end")
    }
}
const uploadScoreImpl = async (score) => {
    upload_error.innerText = ""
    upload_success.hidden = true;
    let name = document.getElementById("username-input").value;
    const response = await fetch(API_ADDRESS_PORT + "/upload_score", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            username: name,
            score: score
        })
    })
    if (response.ok) {
        upload_success.hidden = false;
        await fetchLeaderboard();
    } else {
        const body = JSON.parse(await response.text())
        if (body.type === undefined || body.type !== "Error") {
            console.error(body)
            return;
        }
        if (body.kind === "validation") {
            upload_error.innerText = body.message
        }
    }
}
const setOrGetPersonalBestImpl = (score) => {
    let storage = window.localStorage;
    const storagePersonalBest = Number.parseInt(storage.getItem(MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR))
    if (storagePersonalBest === null) {
        storage.setItem(MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR, score)
    } else if (storagePersonalBest >= score && storagePersonalBest >= 0) {
        return storagePersonalBest;
    } else {
        storage.setItem(MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR, score);
        return score;
    }
}

const fetchLeaderboard = async () => {
    let leaderboard_body = document.getElementById("leaderboard-body")

    const response = await fetch(API_ADDRESS_PORT + `/top_scores?amount=${LEADERBOARD_FETCH_AMOUNT}`, {
        method: "GET",
    })
    if (response.ok) {
        const scores = JSON.parse(await response.text());
        if (scores.type !== "ScoreListResponse") {
            console.error("expected ScoreListResponse, got something different")
            return;
        }
        leaderboard_body.innerHTML = ""
        let i = 1;
        for (const s of scores.scores) {
            leaderboard_body.innerHTML += `<tr><td>${i++}.</td><td>${s.username}</td><td>${s.score}</td></tr>`
        }
    } else {
        console.error("error while trying to fetch leaderboard, got response:")
        console.error(response.body)
    }

}

window.onload = (_) => {
    fetchLeaderboard();
    upload_error = document.getElementById("upload-error")
    upload_success = document.getElementById("upload-success")
    username = document.getElementById("username-input")
    validation_result = document.getElementById("validation-result")

    upload_success.hidden = true;
    username.value = `Ryszard${Math.floor((Math.random() * 10000) % 10000)}`

    document.getElementById("username-input").onclick = () => {
        upload_error.innerText = ""
        upload_success.hidden = true
    };
}

const validateUsername = () => {
    let name = username.value;
    if (name === "" || (name.length < 5 || name.length > 30)) {
        validation_result.innerText = "❌"
    } else {
        validation_result.innerText = "✔️"
    }
}
