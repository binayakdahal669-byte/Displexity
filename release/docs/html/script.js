// Displexity Documentation Scripts

// Smooth scrolling for navigation
document.querySelectorAll('.nav-links a').forEach(link => {
    link.addEventListener('click', (e) => {
        e.preventDefault();
        const target = document.querySelector(link.getAttribute('href'));
        if (target) {
            target.scrollIntoView({ behavior: 'smooth' });
        }
    });
});

// Highlight active section in nav
const sections = document.querySelectorAll('section');
const navLinks = document.querySelectorAll('.nav-links a');

window.addEventListener('scroll', () => {
    let current = '';
    sections.forEach(section => {
        const sectionTop = section.offsetTop;
        if (scrollY >= sectionTop - 100) {
            current = section.getAttribute('id');
        }
    });
    
    navLinks.forEach(link => {
        link.classList.remove('active');
        if (link.getAttribute('href') === '#' + current) {
            link.classList.add('active');
        }
    });
});

// Copy code blocks on click
document.querySelectorAll('pre code').forEach(block => {
    block.addEventListener('click', () => {
        navigator.clipboard.writeText(block.textContent);
        // Visual feedback
        const original = block.style.background;
        block.style.background = '#238636';
        setTimeout(() => {
            block.style.background = original;
        }, 200);
    });
});

console.log('Displexity Documentation loaded');
