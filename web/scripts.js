const MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR = "miskaPersonalBest"
const MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR = "miskaDefaultUsername"
const LEADERBOARD_FETCH_AMOUNT = 15

var upload_error = null;
var upload_success = null;
var username = null;
var validation_result = null;
var validation_tooltip = null;

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
        setTimeout(async () => {
            upload_success.hidden = true;
        }, 1000)
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
        if (body.kind === "unregistered-game-start") {
            upload_error.innerText = "you must start a new game"
        }
    }
}

const heartbeatImpl = async () => {
    const response = await fetch(API_ADDRESS_PORT + "/heartbeat", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json",
        },
    })
    if (!response.ok) {
        console.error("heartbeat failed")
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
    validation_tooltip = document.getElementById("validation-tooltip")

    upload_success.hidden = true;

    const storage = window.localStorage;
    default_username = storage.getItem(MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR)
    if(default_username !== null){
        username.value = default_username
    } else {
        const names = [
            "Ryszard",
            "KamilPatryk",
            "Miśka",
            "DawidJasper",
            "Aro",
            "Boljer",
            "Tubson"
        ]
        const name = names[Math.floor(Math.random() * names.length)]
        username.value = `${name}${Math.floor((Math.random() * 10000) % 10000)}`
        storage.setItem(MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR, username.value)
    }

    document.getElementById("username-input").onclick = () => {
        upload_error.innerText = ""
        upload_success.hidden = true
    };
}

const validateUsername = () => {
    let name = username.value;
    validation_result.innerText = "❌"
    if (name === "") {
        validation_tooltip.innerText = "username cannot be empty"
    } else if (name.length < 5 || name.length > 30) {
        validation_tooltip.innerText = "username must be at least 5 and at most 30 characters long"
    } else if (/\s/g.test(name)) {
        validation_tooltip.innerText = "username cannot contain whitespace"
    } else {
        validation_tooltip.innerText = "username OK"
        validation_result.innerText = "✔️"
        window.localStorage.setItem(MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR, name)
    }
}
