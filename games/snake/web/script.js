const canvas = document.getElementById('game');
const ctx = canvas.getContext('2d');
const controls = document.getElementById('controls');
const gameOverlay = document.getElementById('game-overlay');
const gameOverText = document.getElementById('game-over-text');
const homeScreen = document.getElementById('home-screen');
const startGameBtn = document.getElementById('start-game');
const topBar = document.querySelector('.topbar');
const gameArea = document.getElementById('game-area');
const pauseBtn = document.getElementById('pause-btn');
const pauseMenu = document.getElementById('pause-menu');
const restartBtn = document.getElementById('restart-btn');
const resumeBtn = document.getElementById('resume-btn');
const quitBtn = document.getElementById('quit-btn');
const pauseScoreVal = document.getElementById('pause-score-val');
const pauseHighscoreVal = document.getElementById('pause-highscore-val');
const headerScoreVal = document.getElementById('header-score-val');
const headerHighscoreVal = document.getElementById('header-highscore-val');

const CELL_SIZE = 20;
const config = {
  speed: 5, // 1-10
  tickMs: 150,
  minBorderWidth: 30,
  colors: {
    snakeHead: '#66C7F4',
    snakeBody: '#4A90E2',
    food: '#f52323',
    eye: '#121A12',
  }
};
let cols = 0, rows = 0;

// Colors from the palette in styles.css
const SNAKE_HEAD_COLOR = config.colors.snakeHead;
const SNAKE_BODY_COLOR = config.colors.snakeBody;
const FOOD_COLOR = config.colors.food;
const EYE_COLOR = config.colors.eye;
const MIN_BORDER_WIDTH = config.minBorderWidth; // Minimum width of canvas border in pixels

let snake = [];
let dir = {x: 1, y: 0};
let lastDir = {x: 1, y: 0};
let nextDir = null;
let food = null;
let score = 0;
let highScore = 0;

let lastTickTime = 0;
let isGameOver = false;
let isPaused = false;
let pauseStartTime = 0;
let poppedTail = null;

function resizeCanvas() {
  const rect = canvas.parentElement.getBoundingClientRect();
  const ratio = window.devicePixelRatio || 1;

  const availableWidth = rect.width - MIN_BORDER_WIDTH * 2;
  const availableHeight = rect.height - MIN_BORDER_WIDTH * 2;

  cols = Math.floor(availableWidth / CELL_SIZE);
  rows = Math.floor(availableHeight / CELL_SIZE);

  const finalWidth = cols * CELL_SIZE;
  const finalHeight = rows * CELL_SIZE;
  
  const leftoverX = availableWidth - finalWidth;
  const leftoverY = availableHeight - finalHeight;
  
  const borderWidthX = MIN_BORDER_WIDTH * 2 + leftoverX;
  const borderWidthY = MIN_BORDER_WIDTH * 2 + leftoverY;

  canvas.style.width = `${finalWidth}px`;
  canvas.style.height = `${finalHeight}px`;
  
  canvas.style.borderLeftWidth = canvas.style.borderRightWidth = `${borderWidthX / 2}px`;
  canvas.style.borderTopWidth = canvas.style.borderBottomWidth = `${borderWidthY / 2}px`;
  
  canvas.width = finalWidth * ratio;
  canvas.height = finalHeight * ratio;
  
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
}

function placeFood() {
  let attempts = 0;
  while (attempts++ < 2000) {
    const x = Math.floor(Math.random() * cols);
    const y = Math.floor(Math.random() * rows);
    if (!snake.some(s => s.x === x && s.y === y)) {
      food = {x, y};
      return;
    }
  }
  food = {x: 0, y: 0};
}

function showHomeScreen() {
  homeScreen.style.display = 'flex';
  topBar.style.display = 'none';
  gameArea.style.display = 'none';
}

function startGame() {
  homeScreen.style.display = 'none';
  topBar.style.display = 'flex';
  gameArea.style.display = 'flex';
  resetGameState();
  requestAnimationFrame(gameLoop);
}

function resetGameState() {
  isGameOver = false;
  if (isPaused) {
    isPaused = false;
  }
  gameOverlay.style.display = 'none';
  gameOverlay.classList.remove('game-over-animation');
  pauseMenu.style.display = 'none';
  
  resizeCanvas();
  
  head = { x: Math.floor(cols / 2), y: Math.floor(rows / 2) };
  snake = [
    head,
    { x: head.x - 1, y: head.y },
    { x: head.x - 2, y: head.y }
  ];
  dir = { x: 1, y: 0 };
  lastDir = { x: 1, y: 0 };
  nextDir = null;
  poppedTail = null;
  score = 0;
  config.tickMs = 200 - (config.speed - 1) * 15;
  updateScores();
  placeFood();
  lastTickTime = performance.now();
}

function updateScores(scoreChanged = false, highscoreChanged = false) {
  headerScoreVal.textContent = score;
  headerHighscoreVal.textContent = highScore;

  if (scoreChanged) {
    pulseAnimation(headerScoreVal.previousElementSibling);
  }
  if (highscoreChanged) {
    pulseAnimation(headerHighscoreVal.previousElementSibling);
  }
}

function pulseAnimation(element) {
  element.classList.add('pulse');
  element.addEventListener('animationend', () => {
    element.classList.remove('pulse');
  }, { once: true });
}

function gameOver() {
  isGameOver = true;
  if (score > highScore) {
    highScore = score;
    updateScores(false, true);
  }
  
  gameOverlay.style.display = 'flex';
  gameOverlay.classList.add('game-over-animation');
  
  setTimeout(showGameOverMenu, 1000); // Wait for animation to play twice
}

function showGameOverMenu() {
  gameOverlay.style.display = 'none';
  gameOverlay.classList.remove('game-over-animation');
  
  pauseScoreVal.textContent = score;
  pauseHighscoreVal.textContent = highScore;
  
  resumeBtn.style.display = 'none';
  
  pauseMenu.style.display = 'flex';
}

function tick(currentTime) {
  if (nextDir) {
    if (!(nextDir.x === -dir.x && nextDir.y === -dir.y)) {
      dir = nextDir;
    }
    nextDir = null;
  }
  lastDir = dir;

  let nx = snake[0].x + dir.x;
  let ny = snake[0].y + dir.y;

  if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) {
    gameOver();
    return;
  }

  if (snake.some(s => s.x === nx && s.y === ny)) {
    gameOver();
    return;
  }

  snake.unshift({x: nx, y: ny});

  if (food && nx === food.x && ny === food.y) {
    score++;
    updateScores(true, false);
    placeFood();
    poppedTail = null;
    if (score % 5 === 0 && config.tickMs > 50) {
      config.tickMs -= 10;
    }
  } else {
    poppedTail = snake.pop();
  }
  
  if (score > highScore) {
    highScore = score;
    updateScores(false, true);
  }
}

function interpolate(p1, p2, t) {
    return {
        x: p1.x + (p2.x - p1.x) * t,
        y: p1.y + (p2.y - p1.y) * t
    };
}

function render(currentTime) {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  
  if (isGameOver) {
      return; 
  }

  const interp = Math.min(1, (currentTime - lastTickTime) / config.tickMs);

  // Draw food
  if (food) {
    const foodRadius = CELL_SIZE / 2 * 0.8;
    ctx.beginPath();
    ctx.arc(
        food.x * CELL_SIZE + CELL_SIZE / 2, 
        food.y * CELL_SIZE + CELL_SIZE / 2, 
        foodRadius, 0, Math.PI * 2
    );
    ctx.fillStyle = FOOD_COLOR;
    ctx.fill();
  }

  // --- Calculate interpolated points for the snake path ---
  const points = [];
  
  // Head
  const head = snake[0];
  const old_head_pos = { x: head.x - lastDir.x, y: head.y - lastDir.y };
  points.push(interpolate(old_head_pos, head, interp));

  // Body
  for (let i = 1; i < snake.length; i++) {
    // A joint moves from its old position (snake[i]) to its new one (snake[i-1])
    points.push(interpolate(snake[i], snake[i-1], interp));
  }

  // Disappearing tail tip
  if (poppedTail) {
    // The tip moves from its old position (poppedTail) to its new one (the last joint of the snake)
    points.push(interpolate(poppedTail, snake[snake.length - 1], interp));
  }

  // --- Draw the snake path ---
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  
  if (points.length > 1) {
    const scaledPoints = points.map(p => ({
        x: p.x * CELL_SIZE + CELL_SIZE / 2,
        y: p.y * CELL_SIZE + CELL_SIZE / 2
    }));

    ctx.beginPath();
    ctx.moveTo(scaledPoints[0].x, scaledPoints[0].y);

    if (scaledPoints.length > 2) {
        // Draw a continuous curve through all the points.
        for (let i = 1; i < scaledPoints.length - 1; i++) {
            const p1 = scaledPoints[i];
            const p2 = scaledPoints[i+1];
            const midPoint = {
                x: (p1.x + p2.x) / 2,
                y: (p1.y + p2.y) / 2
            };
            ctx.quadraticCurveTo(p1.x, p1.y, midPoint.x, midPoint.y);
        }
        // Curve to the last point
        const lastJoint = scaledPoints[scaledPoints.length - 2];
        const tail = scaledPoints[scaledPoints.length - 1];
        ctx.quadraticCurveTo(lastJoint.x, lastJoint.y, tail.x, tail.y);
    } else {
        // For a snake of length 2, just draw a straight line.
        ctx.lineTo(scaledPoints[1].x, scaledPoints[1].y);
    }

    ctx.strokeStyle = SNAKE_BODY_COLOR;
    ctx.lineWidth = CELL_SIZE * 0.9;
    ctx.stroke();
  }

  // Draw head circle on top
  const headPoint = points[0];
  if (headPoint) {
    ctx.beginPath();
    ctx.arc(
        headPoint.x * CELL_SIZE + CELL_SIZE / 2, 
        headPoint.y * CELL_SIZE + CELL_SIZE / 2, 
        (CELL_SIZE * 0.9) / 2, 0, Math.PI * 2
    );
    ctx.fillStyle = SNAKE_HEAD_COLOR;
    ctx.fill();
    
    // Draw eyes
    const eyeSize = CELL_SIZE * 0.1;
    const eyeOffset = CELL_SIZE * 0.25;
    const headAngle = Math.atan2(lastDir.y, lastDir.x);
    const headX = headPoint.x * CELL_SIZE + CELL_SIZE / 2;
    const headY = headPoint.y * CELL_SIZE + CELL_SIZE / 2;
    
    const eye1X = headX + Math.cos(headAngle + Math.PI/2) * eyeOffset;
    const eye1Y = headY + Math.sin(headAngle + Math.PI/2) * eyeOffset;
    const eye2X = headX + Math.cos(headAngle - Math.PI/2) * eyeOffset;
    const eye2Y = headY + Math.sin(headAngle - Math.PI/2) * eyeOffset;

    ctx.beginPath();
    ctx.arc(eye1X, eye1Y, eyeSize, 0, Math.PI * 2);
    ctx.arc(eye2X, eye2Y, eyeSize, 0, Math.PI * 2);
    ctx.fillStyle = EYE_COLOR;
    ctx.fill();
  }
}

function gameLoop(currentTime) {
  if (isGameOver || isPaused) {
    requestAnimationFrame(gameLoop);
    return;
  }

  const delta = currentTime - lastTickTime;
  if (delta >= config.tickMs) {
    tick(currentTime);
    lastTickTime = currentTime - (delta % config.tickMs);
  }
  
  render(currentTime);
  requestAnimationFrame(gameLoop);
}

const keyMap = {
  'ArrowUp': {x:0,y:-1}, 'w': {x:0,y:-1},
  'ArrowDown': {x:0,y:1}, 's': {x:0,y:1},
  'ArrowLeft': {x:-1,y:0}, 'a': {x:-1,y:0},
  'ArrowRight': {x:1,y:0}, 'd': {x:1,y:0},
  ' ': 'pause', 'p': 'pause', 'P': 'pause'
};

window.addEventListener('keydown', e => {
  const action = keyMap[e.key];
  if (action) {
    e.preventDefault();
    if (action === 'pause') {
      togglePause();
    } else if (!isPaused) {
      nextDir = action;
    }
  }
});

document.querySelectorAll('.ctl').forEach(btn => {
  btn.addEventListener('click', () => {
    if (isPaused) return;
    const d = btn.getAttribute('data-dir');
    const map = { up:{x:0,y:-1}, down:{x:0,y:1}, left:{x:-1,y:0}, right:{x:1,y:0} }[d];
    if (map) {
      nextDir = map;
    }
  });
});

function togglePause() {
  if (isGameOver) return;
  isPaused = !isPaused;
  if (isPaused) {
    pauseStartTime = performance.now();
    
    pauseScoreVal.textContent = score;
    pauseHighscoreVal.textContent = highScore;
    
    resumeBtn.style.display = 'flex';
    
    pauseMenu.style.display = 'flex';
  } else {
    lastTickTime += (performance.now() - pauseStartTime);
    pauseMenu.style.display = 'none';
  }
}

function handleRestart() {
  resetGameState();
}

function quitGame() {
  if (isPaused) {
    togglePause();
  }
  showHomeScreen();
}

pauseBtn.addEventListener('click', togglePause);
restartBtn.addEventListener('click', handleRestart);
resumeBtn.addEventListener('click', togglePause);
quitBtn.addEventListener('click', quitGame);
startGameBtn.addEventListener('click', startGame);

window.addEventListener('resize', () => {
    if (!isGameOver) {
        resetGameState();
    }
});

showHomeScreen();
