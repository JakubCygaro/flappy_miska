const MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR = "miskaPersonalBest"
const MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR = "miskaDefaultUsername"
const LEADERBOARD_FETCH_AMOUNT = 15

var uploadError = null;
var uploadSuccess = null;
var username = null;
var validationResult = null;
var validationTooltip = null;
var suppressHeartbeat = false;

const registerGameImpl = async () => {
    const response = await fetch(API_ADDRESS_PORT + "/register_game", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json",
        },
    })
    if (response.ok) {
        suppressHeartbeat = false;
    } else {
        console.error("failed to register game start")
        suppressHeartbeat = true;
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
    uploadError.innerText = ""
    uploadSuccess.hidden = true;
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
        uploadSuccess.hidden = false;
        setTimeout(async () => {
            uploadSuccess.hidden = true;
        }, 1000)
        await fetchLeaderboard();
    } else {
        const body = JSON.parse(await response.text())
        if (body.type === undefined || body.type !== "Error") {
            console.error(body)
            return;
        }
        if (body.kind === "validation") {
            uploadError.innerText = body.message
        }
        if (body.kind === "unregistered-game-start") {
            uploadError.innerText = "you must start a new game"
        }
    }
}

const heartbeatImpl = async () => {
    if (suppressHeartbeat) return;
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
    uploadError = document.getElementById("upload-error")
    uploadSuccess = document.getElementById("upload-success")
    username = document.getElementById("username-input")
    validationResult = document.getElementById("validation-result")
    validationTooltip = document.getElementById("validation-tooltip")

    uploadSuccess.hidden = true;

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
        uploadError.innerText = ""
        uploadSuccess.hidden = true
    };
}

const validateUsername = () => {
    let name = username.value;
    validationResult.innerText = "❌"
    if (name === "") {
        validationTooltip.innerText = "username cannot be empty"
    } else if (name.length < 5 || name.length > 30) {
        validationTooltip.innerText = "username must be at least 5 and at most 30 characters long"
    } else if (/\s/g.test(name)) {
        validationTooltip.innerText = "username cannot contain whitespace"
    } else {
        validationTooltip.innerText = "username OK"
        validationResult.innerText = "✔️"
        window.localStorage.setItem(MISKA_DEFAULT_USERNAME_LOCAL_STORAGE_VAR, name)
    }
}
