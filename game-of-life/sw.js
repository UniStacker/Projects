const CACHE_NAME = 'bio-life-v1';
const ASSETS = [
  '/',
  '/index.html',
  '/style.css',
  '/script.js',
  '/manifest.json',
  '/assets/glider.svg',
  '/assets/pulsar.svg',
  '/assets/lwss.svg',
  '/assets/gosper.svg',
  '/assets/dna-toggle.svg',
  '/assets/clear.svg',
  '/assets/random.svg',
  '/assets/start.svg',
  '/assets/speed-vein.svg',
  '/assets/background.svg',
  '/assets/ambient.wav',
  '/assets/icon.svg'
];

// Install and cache all assets
self.addEventListener('install', e => {
  e.waitUntil(
    caches.open(CACHE_NAME).then(cache => cache.addAll(ASSETS))
  );
});

// Serve cached assets if offline
self.addEventListener('fetch', e => {
  e.respondWith(
    caches.match(e.request).then(res => res || fetch(e.request))
  );
});

// Delete old caches on activate
self.addEventListener('activate', e => {
  e.waitUntil(
    caches.keys().then(keys => Promise.all(
      keys.filter(k => k !== CACHE_NAME).map(k => caches.delete(k))
    ))
  );
  self.clients.claim();
});
