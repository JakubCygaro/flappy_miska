const API_ADDRESS_PORT = "http://127.0.0.1:8080"

const registerGameImpl = async () => {
    const response = await fetch(API_ADDRESS_PORT + "/register_game", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type" : "application/json",
        },
    })
    if (response.ok){
        console.log("got session cookie")
    } else {
        console.error("something fucked up")
    }
}

const uploadScoreImpl = async (score) => {
    let username = document.getElementById("username-input").value;
    username = username === "" ? "Ryszard" : username;
    const response = await fetch(API_ADDRESS_PORT + "/upload_score", {
        method: "POST",
        credentials: "include",
        headers: {
            "Content-Type" : "application/json"
        },
        body: JSON.stringify({
            username: username,
            score: score
        })
    })
    if (response.ok){
        console.log("score uploaded")
    } else {
        console.error("something fucked up2")
    }
}

