const API_ADDRESS_PORT = "http://127.0.0.1:8080"
const MISKA_PERSONAL_BEST_LOCAL_STORAGE_VAR = "miskaPersonalBest"

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
    let username = document.getElementById("username-input").value;
    username = username === "" ? "Ryszard" : username;
    const response = await fetch(API_ADDRESS_PORT + "/upload_score", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            username: username,
            score: score
        })
    })
    if (response.ok) {
        await fetchLeaderboard();
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

    const response = await fetch(API_ADDRESS_PORT + "/top_scores?amount=10", {
        method: "GET",
    })
    if (response.ok) {
        const scores = JSON.parse(await response.text());
        if (scores.type !== "ScoreListResponse") {
            console.error("expected ScoreListResponse, got something different")
            return;
        }
        leaderboard_body.innerHTML = ""
        for (const s of scores.scores) {
            leaderboard_body.innerHTML += `<tr><td>${s.username}</td><td>${s.score}</td></tr>`
        }
    } else {
        console.error("error while trying to fetch leaderboard, got response:")
        console.error(response.body)
    }

}

window.onload = (_) => {
    fetchLeaderboard();
}
