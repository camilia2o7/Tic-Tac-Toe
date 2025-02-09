#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <map>
#include <string>
const char* ssid = ""; // Your WiFi Username (SSID)
const char* password = ""; // Your WiFi Password
WebServer server(80);
struct Game {
  char board[3][3];
  char currentPlayer;
  bool active;
  String player1;
  String player2;
};
std::map<String, Game> games;
void handleRoot();
void handleNewGame();
void handleJoinGame();
void handleMove();
void handleStatus();
void sendGameState(String gameCode, String msg);
String generateGameCode() {
  String code = "";
  for (int i = 0; i < 4; i++) {
    code += char('A' + random(26));
  }
  return code;
}
bool checkWin(char board[3][3], char player) {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
    if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
  }
  if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
  if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
  return false;
}
bool isBoardFull(char board[3][3]) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (board[i][j] == ' ') return false;
    }
  }
  return true;
}
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/newGame", HTTP_POST, handleNewGame);
  server.on("/joinGame", HTTP_POST, handleJoinGame);
  server.on("/move", HTTP_POST, handleMove);
  server.on("/status", handleStatus);
  server.on("/restartGame", HTTP_POST, handleRestartGame);
  server.begin();

  randomSeed(analogRead(0));
}
void handleRestartGame() {
    if (!server.hasArg("plain")) {
        server.send(400, "application/json", "{\"error\":\"Missing game code\"}");
        return;
    }
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, server.arg("plain"));
    String gameCode = doc["gameCode"];
    if (games.find(gameCode) != games.end()) {
        Game& game = games[gameCode];
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                game.board[i][j] = ' ';
            }
        }
        game.currentPlayer = 'X';
        game.active = true;
        sendGameState(gameCode, "Game restarted. Player X's turn.");
    } else {
        server.send(400, "application/json", "{\"error\":\"Game not found\"}");
    }
}
void loop() {
  server.handleClient();
}
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tic-Tac-Toe</title>
    <style>
    :root {
    --bg: #120458;
    --primary: #00ff87;
    --secondary: #ff00a0;
    --accent: #ffd700;
    --text: #ffffff;
}

body::-webkit-scrollbar {
    display: none;
}

body {
    font-family: 'Space Grotesk', sans-serif;
    background: linear-gradient(135deg, var(--bg), #3b0764);
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    margin: 0;
    color: var(--text);
}

.container {
    background-color: rgba(255, 255, 255, 0.1);
    backdrop-filter: blur(10px);
    border-radius: 20px;
    padding: 30px;
    box-shadow: 0 10px 30px rgba(0,0,0,0.3), 0 0 20px var(--primary);
    text-align: center;
    max-width: 400px;
    width: 100%;
    transition: all 0.3s ease;
}

.container:hover {
    transform: translateY(-5px);
    box-shadow: 0 15px 40px rgba(0,0,0,0.4), 0 0 30px var(--primary);
}

h1 {
    color: var(--accent);
    margin-bottom: 20px;
    text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
    font-size: 2.5em;
}

.board {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 10px;
    margin: 20px 0;
    position: relative;
}

.cell {
    aspect-ratio: 1;
    background-color: rgba(255, 255, 255, 0.1);
    border: 2px solid var(--primary);
    border-radius: 10px;
    font-size: 40px;
    display: flex;
    justify-content: center;
    align-items: center;
    cursor: pointer;
    transition: all 0.3s ease;
    color: var(--text);
    text-shadow: 0 0 10px var(--primary);
}

.cell:hover {
    background-color: rgba(255, 255, 255, 0.2);
    transform: scale(1.05);
    box-shadow: 0 0 15px var(--primary);
}

#message {
    margin-top: 20px;
    font-size: 18px;
    font-weight: bold;
    color: var(--secondary);
    height: 24px;
    text-shadow: 0 0 5px var(--secondary);
}

input, button {
    margin: 10px;
    padding: 12px 20px;
    font-size: 16px;
    border: none;
    border-radius: 5px;
    background-color: rgba(255, 255, 255, 0.2);
    color: var(--text);
    transition: all 0.3s ease;
}

input {
    width: 60%;
    border: 2px solid var(--secondary);
}

input::placeholder {
    color: rgba(255, 255, 255, 0.7);
}

button {
    cursor: pointer;
    background-color: var(--primary);
    box-shadow: 0 0 10px var(--primary);
    font-weight: bold;
}

button:hover {
    background-color: var(--secondary);
    transform: translateY(-2px);
    box-shadow: 0 0 20px var(--secondary);
}

#gameCode {
    font-size: 24px;
    font-weight: bold;
    color: var(--accent);
    margin: 20px 0;
    text-shadow: 0 0 10px var(--accent);
}

#gameSetup, #gameBoard {
    transition: opacity 0.5s ease-in-out;
}

#gameBoard {
    display: none;
}

@keyframes celebrate {
    0% { transform: scale(1) rotate(0deg); }
    50% { transform: scale(1.1) rotate(5deg); }
    100% { transform: scale(1) rotate(0deg); }
}

.winner {
    animation: celebrate 0.5s ease-in-out 3;
}

#winLine {
    position: absolute;
    background-color: var(--accent);
    transition: all 0.5s ease;
    box-shadow: 0 0 20px var(--accent);
}

.confetti {
    position: absolute;
    width: 10px;
    height: 10px;
    background-color: var(--secondary);
    animation: confetti-fall 3s ease-out infinite;
}

@keyframes confetti-fall {
    0% { transform: translateY(-100vh) rotate(0deg); }
    100% { transform: translateY(100vh) rotate(720deg); }
}

@media (max-width: 768px) {
    .container {
        transform: scale(0.9);
        transform-origin: center;
    }
}

@media (max-width: 600px) { 
    .container {
        transform: scale(0.9);
        transform-origin: center;
    }
}

@media (max-width: 400px) {
    .container {
        transform: scale(0.9);
        transform-origin: center;
    }
}
    </style>
    <link href="https://fonts.googleapis.com/css2?family=Poppins:wght@400;700&display=swap" rel="stylesheet">
</head>
<body>
    <div class="container">
        <h1>Tik-Tac-Toe</h1>
        <div id="gameSetup">
            <button id="newGameBtn">New Game</button>
            <input type="text" id="joinGameCode" placeholder="Enter game code">
            <button id="joinGameBtn">Join Game</button>
        </div>
        <div id="gameBoard">
            <div id="gameCode"></div>
            <div class="board" id="board">
                <div id="winLine"></div>
            </div>
            <div id="message"></div>
            <button id="restartBtn" style="display: none;">Restart Game</button>
        </div>
    </div>
    <script>
        const gameSetup = document.getElementById('gameSetup');
        const gameBoard = document.getElementById('gameBoard');
        const board = document.getElementById('board');
        const message = document.getElementById('message');
        const newGameBtn = document.getElementById('newGameBtn');
        const joinGameBtn = document.getElementById('joinGameBtn');
        const joinGameCode = document.getElementById('joinGameCode');
        const gameCodeDisplay = document.getElementById('gameCode');
        const restartBtn = document.getElementById('restartBtn');
        const winLine = document.getElementById('winLine');
        let gameCode = '';
        let playerSymbol = '';
        function createBoard() {
            board.innerHTML = '<div id="winLine"></div>';
            for (let i = 0; i < 9; i++) {
                const cell = document.createElement('div');
                cell.classList.add('cell');
                cell.dataset.index = i;
                cell.addEventListener('click', handleCellClick);
                board.appendChild(cell);
            }
        }
        function handleCellClick(e) {
            if (!gameCode) return;
            const index = e.target.dataset.index;
            makeMove(index);
        }
        function makeMove(index) {
            fetch('/move', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ gameCode, index: parseInt(index), player: playerSymbol }),
            })
            .then(response => response.json())
            .then(updateGame)
            .catch(error => console.error('Error:', error));
        }
        function updateGame(data) {
            if (!data.game) return;
            const cells = document.getElementsByClassName('cell');
            for (let i = 0; i < 9; i++) {
                cells[i].textContent = data.game.board[Math.floor(i / 3)][i % 3];
            }
            message.textContent = data.message;
            if (data.message.includes("wins")) {
                celebrateWin(data.game.board);
            } else if (data.message.includes("draw")) {
                showDraw();
            }
            restartBtn.style.display = data.game.active ? 'none' : 'inline-block';
        }
        function celebrateWin(board) {
            const winningLine = findWinningLine(board);
            if (winningLine) {
                drawWinningLine(winningLine);
            }
            createConfetti();
        }
        function findWinningLine(board) {
            const lines = [
                [0, 1, 2], [3, 4, 5], [6, 7, 8], // Rows
                [0, 3, 6], [1, 4, 7], [2, 5, 8], // Columns
                [0, 4, 8], [2, 4, 6] // Diagonals
            ];
            for (let line of lines) {
                const [a, b, c] = line;
                if (board[Math.floor(a/3)][a%3] &&
                    board[Math.floor(a/3)][a%3] === board[Math.floor(b/3)][b%3] &&
                    board[Math.floor(a/3)][a%3] === board[Math.floor(c/3)][c%3]) {
                    return line;
                }
            }
            return null;
        }
        function drawWinningLine(line) {
            const cellSize = board.offsetWidth / 3;
            const [start, _, end] = line;
            const startX = (start % 3) * cellSize + cellSize / 2;
            const startY = Math.floor(start / 3) * cellSize + cellSize / 2;
            const endX = (end % 3) * cellSize + cellSize / 2;
            const endY = Math.floor(end / 3) * cellSize + cellSize / 2;
            const length = Math.sqrt(Math.pow(endX - startX, 2) + Math.pow(endY - startY, 2));
            const angle = Math.atan2(endY - startY, endX - startX) * 180 / Math.PI;
            winLine.style.width = `${length}px`;
            winLine.style.height = '5px';
            winLine.style.top = `${startY}px`;
            winLine.style.left = `${startX}px`;
            winLine.style.transform = `rotate(${angle}deg)`;
            winLine.style.transformOrigin = 'left';
        }
        function createConfetti() {
    const colors = ['#FF5733', '#33FF57', '#3357FF', '#FF33A1', '#FFF233'];
    const shapes = ['circle', 'square', 'triangle'];
    for (let i = 0; i < 100; i++) {
        const confetti = document.createElement('div');
        const size = Math.random() * 10 + 5; // Random size between 5px and 15px
        const color = colors[Math.floor(Math.random() * colors.length)];
        const shape = shapes[Math.floor(Math.random() * shapes.length)];
        confetti.style.position = 'absolute';
        confetti.style.width = `${size}px`;
        confetti.style.height = `${size}px`;
        confetti.style.backgroundColor = color;
        confetti.style.borderRadius = shape === 'circle' ? '50%' : '0';
        confetti.style.left = `${Math.random() * 100}%`;
        confetti.style.top = `${Math.random() * -20}vh`;
        confetti.style.animation = `fall ${Math.random() * 3 + 2}s ease-out, rotate ${Math.random() * 2 + 1}s linear infinite`;
        document.body.appendChild(confetti);
        setTimeout(() => {
            confetti.remove();
        }, 5000);
    }
}
      const styleElement = document.createElement('style');
      styleElement.textContent = `
          @keyframes fall {
              to {
                  transform: translateY(100vh);
              }
          }
          @keyframes rotate {
              to {
                  transform: rotate(360deg);
              }
          }
      `;
document.head.appendChild(styleElement);
        function showDraw() {
            message.textContent = "It's a draw!";
            message.style.color = '#FFD700';
        }
        function checkStatus() {
            if (!gameCode) return;
            fetch(`/status?gameCode=${gameCode}`)
            .then(response => response.json())
            .then(updateGame)
            .catch(error => console.error('Error:', error));
        }
        function startGame() {
            gameSetup.style.opacity = '0';
            setTimeout(() => {
                gameSetup.style.display = 'none';
                gameBoard.style.display = 'block';
                setTimeout(() => {
                    gameBoard.style.opacity = '1';
                }, 50);
            }, 500);
        }
        newGameBtn.addEventListener('click', () => {
            console.log('New Game button clicked');
            fetch('/newGame', { method: 'POST' })
            .then(response => response.json())
            .then(data => {
                console.log('New game data received:', data);
                gameCode = data.gameCode;
                playerSymbol = 'X';
                gameCodeDisplay.textContent = `Game Code: ${gameCode}`;
                createBoard();
                message.textContent = 'Waiting for player 2 to join...';
                startGame();
            })
            .catch(error => console.error('Error:', error));
        });
        joinGameBtn.addEventListener('click', () => {
            console.log('Join Game button clicked');
            gameCode = joinGameCode.value.toUpperCase();
            fetch('/joinGame', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ gameCode }),
            })
            .then(response => response.json())
            .then(data => {
                console.log('Join game data received:', data);
                if (data.success) {
                    playerSymbol = 'O';
                    gameCodeDisplay.textContent = `Game Code: ${gameCode}`;
                    createBoard();
                    message.textContent = 'Game joined! Waiting for your turn...';
                    startGame();
                                } else {
                    message.textContent = 'Failed to join game. Please check the game code.';
                }
            })
            .catch(error => console.error('Error:', error));
        });
        restartBtn.addEventListener('click', () => {
    console.log('Restart Game button clicked');
    fetch('/restartGame', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ gameCode }),
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Failed to restart game: ${response.statusText}`);
        }
        return response.json();
    })
    .then(data => {
        console.log('Game restarted:', data);
        createBoard();
        message.textContent = data.message;
        winLine.style.width = '0';
        restartBtn.style.display = 'none';
    })
    .catch(error => {
        console.error('Error:', error);
        message.textContent = 'Failed to restart game. Please try again.';
    });
});
        setInterval(checkStatus, 1000);
    </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}
void handleNewGame() {
  Serial.println("New game requested");
  String gameCode = generateGameCode();
  games[gameCode] = {
    {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}},
    'X',
    true,
    server.client().remoteIP().toString(),
    ""
  };
  DynamicJsonDocument doc(1024);
  doc["gameCode"] = gameCode;
  String response;
  serializeJson(doc, response);
  Serial.println("Sending new game response: " + response);
  server.send(200, "application/json", response);
}
void handleJoinGame() {
  Serial.println("Join game requested");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  String gameCode = doc["gameCode"];
  if (games.find(gameCode) != games.end() && games[gameCode].player2.isEmpty()) {
    games[gameCode].player2 = server.client().remoteIP().toString();
    Serial.println("Game joined successfully");
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    Serial.println("Failed to join game");
    server.send(200, "application/json", "{\"success\":false}");
  }
}
void handleMove() {
  Serial.println("Move requested");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, server.arg("plain"));
  String gameCode = doc["gameCode"];
  int index = doc["index"];
  String player = doc["player"];
  if (games.find(gameCode) != games.end()) {
    Game& game = games[gameCode];
    int row = index / 3;
    int col = index % 3;
    if (game.board[row][col] == ' ' && game.active && player[0] == game.currentPlayer) {
      game.board[row][col] = game.currentPlayer;
      if (checkWin(game.board, game.currentPlayer)) {
        game.active = false;
        sendGameState(gameCode, player + " wins!");
      } else if (isBoardFull(game.board)) {
        game.active = false;
        sendGameState(gameCode, "It's a draw!");
      } else {
        game.currentPlayer = (game.currentPlayer == 'X') ? 'O' : 'X';
        sendGameState(gameCode, "Next player's turn");
      }
    } else {
      sendGameState(gameCode, "Invalid move");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"Game not found\"}");
  }
}
void handleStatus() {
  String gameCode = server.arg("gameCode");
  if (games.find(gameCode) != games.end()) {
    sendGameState(gameCode, "Game status updated");
  } else {
    server.send(400, "application/json", "{\"error\":\"Game not found\"}");
  }
}
void sendGameState(String gameCode, String msg) {
  DynamicJsonDocument doc(1024);
  JsonObject gameObj = doc.createNestedObject("game");
  JsonArray boardArray = gameObj.createNestedArray("board");
  for (int i = 0; i < 3; i++) {
    JsonArray row = boardArray.createNestedArray();
    for (int j = 0; j < 3; j++) {
      row.add(String(games[gameCode].board[i][j]));
    }
  }
  gameObj["currentPlayer"] = String(games[gameCode].currentPlayer);
  gameObj["active"] = games[gameCode].active;
  doc["message"] = msg;
  String response;
  serializeJson(doc, response);
  Serial.println("Sending game state: " + response);
  server.send(200, "application/json", response);
}