self.addEventListener('install', (e) => {
  e.waitUntil(
    caches.open('magicdoor').then((cache) => cache.addAll([
      '/',
      '/index.html',
      '/assets/vendor/nucleo/css/nucleo.css',
      '/assets/vendor/@fortawesome/fontawesome-free/css/all.min.css',
      '/assets/css/argon.css?v=1.1.0'
    ])),
  );
});

self.addEventListener('fetch', (e) => {
  console.log(e.request.url);
  e.respondWith(
    caches.match(e.request).then((response) => response || fetch(e.request)),
  );
});
