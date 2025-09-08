// DOM elements
const canvas = document.getElementById("gameCanvas");
const joystick = document.getElementById("joystick");
const stick = document.getElementById("stick");

// Canvas setup
const ctx = canvas.getContext('2d');
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

// Keyboard setup
const keys = {};
window.addEventListener("keydown", e => keys[e.key] = true);
window.addEventListener("keyup", e => keys[e.key] = false);

// Paddle setup
const paddleWidth = 25;
const paddleHeight = 100;
let playerY = canvas.height / 2 - paddleHeight / 2;
let aiY = playerY

// Ball setup
const ballSize = 20;
let ballX = canvas.width / 2 - ballSize / 2;
let ballY = canvas.height / 2 - ballSize / 2;
let ballSpeedX = 5;
let ballSpeedY = 5;

// Joystick setup
joyActive = false
joyOffsetY = 0
joystick.addEventListener("touchstart", e => joyActive = true);
joystick.addEventListener("touchend", e => {
  joyActive = false;
  joyOffsetY = 0;
  stick.style.left = "calc(50% - 0.75em)";
  stick.style.bottom = "calc(50% - 0.75em)";
});
joystick.addEventListener("touchmove", e => {
  e.preventDefault();
  if (joyActive) {
    let touch = e.touches[0];
    let rect = joystick.getBoundingClientRect();
    let relX = (touch.clientX - rect.left) / rect.width - 0.5;
    let relY = (touch.clientY - rect.top) / rect.height - 0.5;
    
    maxDistance = 0.4;
    relX = Math.max(-maxDistance, Math.min(maxDistance, relX));
    relY = Math.max(-maxDistance, Math.min(maxDistance, relY))
    joyOffsetY = relY * 2; // -0.8 to 0.8 -> Scale to -1..1
    stick.style.left = `calc(50% + ${relX * 50}%)`
    stick.style.top = `calc(50% + ${relY * 50}%)`;
  }
}, {passive: false});


// Clamps the y coordinate of a paddle to stay within screen bounds
function clampedY(y) {
  return Math.max(0, Math.min(y, canvas.height - paddleHeight));
}

function draw() {
  // 1. Clear screen
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // 2. Draw player
  ctx.fillStyle = "lightgreen";
  ctx.fillRect(0, playerY, paddleWidth, paddleHeight);

  // 3. Draw ai
  ctx.fillStyle = "salmon";
  ctx.fillRect(canvas.width - paddleWidth, aiY, paddleWidth, paddleHeight);

  // 3. Draw ball
  ctx.fillStyle = "white";
  ctx.fillRect(ballX, ballY, ballSize, ballSize);
}

function updatePlayer() {
  let speed = joyOffsetY * 15; // Scaled to joystick movement
  if (Math.abs(speed) < 1) speed = 0; // dead zone

  // fallback to keyboard
  if (keys['ArrowUp']) speed = -7;
  if (keys['ArrowDown']) speed = 7;

  playerY += speed;

  playerY = clampedY(playerY);
}

function updateAi() {
  let speed = 4;
  let center = aiY + paddleHeight / 2;
  let ballCenter = ballY + ballSize / 2;

  if (center < ballCenter) aiY += speed;
  else if (center > ballCenter) aiY -= speed;

  aiY = clampedY(aiY);
}

function checkScore() {
  if (ballX < 0 || ballX > canvas.width) {
    ballX = canvas.width / 2 - ballSize / 2;
    ballY = canvas.height / 2 - ballSize / 2;
    ballSpeedX = -ballSpeedX;
  }
}

function updateBall() {
  // 1. Handle Y movement
  if (ballY <= 0 || ballY >= canvas.height) ballSpeedY = -ballSpeedY;
  ballY += ballSpeedY;

  // 2. Handle paddle collision (player)
  if (ballX <= paddleWidth && ballY + ballSize >= playerY && ballY <= playerY + paddleHeight) {
    ballSpeedX = -ballSpeedX;
    ballX = paddleWidth;
  }

  // 3. Handle paddle collision (ai)
  if (ballX >= canvas.width - paddleWidth && ballY + ballSize >= aiY && ballY <= aiY + paddleHeight) {
    ballSpeedX = -ballSpeedX;
    ballX = canvas.width - paddleWidth;
  }

  // 4. Handle X movement
  ballX += ballSpeedX;

  // 5. Check score
  checkScore();
}

function gameLoop() {
  updateBall();
  updatePlayer();
  updateAi();
  draw();
  requestAnimationFrame(gameLoop);
}

gameLoop();
