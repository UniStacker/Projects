const gridWrapper = document.getElementById('gridWrapper');
const gridContainer = document.getElementById('grid');
const startBtn = document.getElementById('start');
const randomBtn = document.getElementById('random');
const clearBtn = document.getElementById('clear');
const speedBtn = document.getElementById('speed');
const dnaBtn = document.getElementById('dna');
const ambientAudio = document.getElementById('ambient');
const dnaModal = document.getElementById('dnaModal');
const closeModalBtn = document.getElementById('closeModal');
const patternBtns = document.querySelectorAll('.pattern-btn');

let world = {}; // Infinite grid storage: { "x,y": 1 }
let cellSize = 20; // px size of each cell
let scale = 1; // Zoom level
let running = false;
let speed = 200; // Default simulation speed in ms
let interval;
let viewWidth, viewHeight;
let renderRange = 2; // extra cells beyond viewport for margin

const patterns = {
    glider: [[1, 0], [2, 1], [0, 2], [1, 2], [2, 2]],
    lwss: [[1, 0], [4, 0], [0, 1], [0, 2], [4, 2], [0, 3], [1, 3], [2, 3], [3, 3]],
    pulsar: [
        [2, 0], [3, 0], [4, 0], [8, 0], [9, 0], [10, 0],
        [0, 2], [5, 2], [7, 2], [12, 2],
        [0, 3], [5, 3], [7, 3], [12, 3],
        [0, 4], [5, 4], [7, 4], [12, 4],
        [2, 5], [3, 5], [4, 5], [8, 5], [9, 5], [10, 5],
        [2, 7], [3, 7], [4, 7], [8, 7], [9, 7], [10, 7],
        [0, 8], [5, 8], [7, 8], [12, 8],
        [0, 9], [5, 9], [7, 9], [12, 9],
        [0, 10], [5, 10], [7, 10], [12, 10],
        [2, 12], [3, 12], [4, 12], [8, 12], [9, 12], [10, 12]
    ],
    gosper: [
        [24, 0], [22, 1], [24, 1], [12, 2], [13, 2], [20, 2], [21, 2], [34, 2], [35, 2],
        [11, 3], [15, 3], [20, 3], [21, 3], [34, 3], [35, 3], [0, 4], [1, 4], [10, 4],
        [16, 4], [20, 4], [21, 4], [0, 5], [1, 5], [10, 5], [14, 5], [16, 5], [17, 5],
        [22, 5], [24, 5], [10, 6], [16, 6], [24, 6], [11, 7], [15, 7], [12, 8], [13, 8]
    ]
};

function getKey(x, y) {
    return `${x},${y}`;
}

function setCell(x, y, alive) {
    if (alive) {
        world[getKey(x, y)] = 1;
    } else {
        delete world[getKey(x, y)];
    }
}

function getCell(x, y) {
    return world[getKey(x, y)] || 0;
}

function renderGrid() {
    gridContainer.innerHTML = '';

    const scrollLeft = gridWrapper.scrollLeft / scale;
    const scrollTop = gridWrapper.scrollTop / scale;

    const startX = Math.floor(scrollLeft / cellSize) - renderRange;
    const startY = Math.floor(scrollTop / cellSize) - renderRange;
    const endX = Math.ceil((scrollLeft + viewWidth) / cellSize) + renderRange;
    const endY = Math.ceil((scrollTop + viewHeight) / cellSize) + renderRange;

    gridContainer.style.transform = `scale(${scale})`;

    for (let y = startY; y <= endY; y++) {
        for (let x = startX; x <= endX; x++) {
            const alive = getCell(x, y);
            const cell = document.createElement('div');
            cell.classList.add('cell');
            if (alive) {
                cell.classList.add('alive');
            }

            cell.style.position = 'absolute';
            cell.style.left = `${x * cellSize}px`;
            cell.style.top = `${y * cellSize}px`;
            cell.style.width = `${cellSize}px`;
            cell.style.height = `${cellSize}px`;

            cell.dataset.x = x;
            cell.dataset.y = y;

            cell.addEventListener('click', () => {
                const current = getCell(x, y);
                setCell(x, y, current ? 0 : 1);
                renderGrid();
            });

            gridContainer.appendChild(cell);
        }
    }
}

function resize() {
    viewWidth = gridWrapper.clientWidth;
    viewHeight = gridWrapper.clientHeight;
    renderGrid();
}

gridContainer.style.width = `${100000 * cellSize}px`;
gridContainer.style.height = `${100000 * cellSize}px`;
gridWrapper.scrollLeft = gridContainer.offsetWidth / 2;
gridWrapper.scrollTop = gridContainer.offsetHeight / 2;

gridWrapper.addEventListener('wheel', (e) => {
    if (e.ctrlKey || e.metaKey) {
        e.preventDefault();
        scale += (e.deltaY > 0 ? -0.1 : 0.1);
        scale = Math.max(0.2, Math.min(scale, 4));
        renderGrid();
    }
});

function nextGen() {
    const newWorld = {};
    const neighbors = {};

    for (const key in world) {
        const [x, y] = key.split(',').map(Number);

        for (let i = -1; i <= 1; i++) {
            for (let j = -1; j <= 1; j++) {
                if (i === 0 && j === 0) continue;
                const nx = x + i, ny = y + j;
                const nKey = getKey(nx, ny);
                neighbors[nKey] = (neighbors[nKey] || 0) + 1;
            }
        }
    }

    for (const nKey in neighbors) {
        const aliveNeighbors = neighbors[nKey];
        const isAlive = world[nKey] ? 1 : 0;

        if (isAlive && (aliveNeighbors === 2 || aliveNeighbors === 3)) {
            newWorld[nKey] = 1;
        } else if (!isAlive && aliveNeighbors === 3) {
            newWorld[nKey] = 1;
        }
    }

    world = newWorld;
    renderGrid();
}

function randomizeGrid() {
    const scrollLeft = gridWrapper.scrollLeft / scale;
    const scrollTop = gridWrapper.scrollTop / scale;
    const startX = Math.floor(scrollLeft / cellSize);
    const startY = Math.floor(scrollTop / cellSize);
    const endX = startX + Math.ceil(viewWidth / (cellSize * scale));
    const endY = startY + Math.ceil(viewHeight / (cellSize * scale));

    for (let y = startY; y <= endY; y++) {
        for (let x = startX; x <= endX; x++) {
            if (Math.random() > 0.7) {
                setCell(x, y, 1);
            }
        }
    }
    renderGrid();
}

function clearGrid() {
    world = {};
    renderGrid();
}

function toggleRunning() {
    running = !running;
    if (running) {
        interval = setInterval(nextGen, speed);
        startBtn.classList.add('active');
        playAmbient();
    } else {
        clearInterval(interval);
        startBtn.classList.remove('active');
        pauseAmbient();
    }
}

function cycleSpeed() {
    speed = speed === 200 ? 100 : speed === 100 ? 50 : 200;
    if (running) {
        clearInterval(interval);
        interval = setInterval(nextGen, speed);
    }
    speedBtn.classList.toggle('fast', speed === 50);
}

function playAmbient() {
    ambientAudio.volume = 0.2;
    ambientAudio.play().catch(() => {
        document.addEventListener('click', enableAudioOnce, { once: true });
    });
}
function pauseAmbient() {
    ambientAudio.pause();
}
function enableAudioOnce() {
    ambientAudio.play();
}

function showModal() {
    dnaModal.style.display = 'block';
}

function closeModal() {
    dnaModal.style.display = 'none';
}

function loadPattern(patternName) {
    const pattern = patterns[patternName];
    if (!pattern) return;

    const scrollLeft = gridWrapper.scrollLeft / scale;
    const scrollTop = gridWrapper.scrollTop / scale;
    const startX = Math.floor(scrollLeft / cellSize) + Math.floor(viewWidth / (2 * cellSize * scale));
    const startY = Math.floor(scrollTop / cellSize) + Math.floor(viewHeight / (2 * cellSize * scale));

    pattern.forEach(([x, y]) => {
        setCell(startX + x, startY + y, 1);
    });
    renderGrid();
    closeModal();
}

startBtn.addEventListener('click', toggleRunning);
randomBtn.addEventListener('click', randomizeGrid);
clearBtn.addEventListener('click', clearGrid);
speedBtn.addEventListener('click', cycleSpeed);
dnaBtn.addEventListener('click', showModal);
closeModalBtn.addEventListener('click', closeModal);
gridWrapper.addEventListener('scroll', renderGrid);
window.addEventListener('resize', resize);

patternBtns.forEach(btn => {
    btn.addEventListener('click', () => {
        loadPattern(btn.dataset.pattern);
    });
});

resize();
