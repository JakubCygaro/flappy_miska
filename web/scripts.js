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
        console.error("something fucked up")
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
        console.log("score uploaded")
    } else {
        console.error("something fucked up2")
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

