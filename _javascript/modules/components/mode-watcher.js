/**
 * Add listener for theme mode toggle
 */
const toggleElem = document.getElementById('mode-toggle');

export function modeWatcher() {
  if (!toggleElem) {
    return;
  }

  toggleElem.addEventListener('click', () => {
    modeToggle.flipMode();
  });
}
