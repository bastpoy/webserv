// Fonctionnalité de changement de thème
const themeToggle = document.getElementById('theme-toggle');
let isDark = false;

themeToggle.addEventListener('click', () => {
	isDark = !isDark;
	document.body.classList.toggle('dark-theme');
	themeToggle.textContent = isDark ? 'Thème clair' : 'Thème sombre';
});

// Animation des images de la galerie
const images = document.querySelectorAll('.gallery img');
images.forEach(img => {
	img.addEventListener('click', () => {
		img.style.transform = 'scale(1.1)';
		setTimeout(() => {
			img.style.transform = 'scale(1)';
		}, 500);
	});
});

// Navigation fluide modifiée pour ne gérer que les liens internes
document.querySelectorAll('nav a').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
        // Vérifie si le lien commence par # (lien interne)
        if (this.getAttribute('href').startsWith('#')) {
            e.preventDefault();
            const targetId = this.getAttribute('href').substring(1);
            const targetElement = document.getElementById(targetId);
            if (targetElement) {
                targetElement.scrollIntoView({
                    behavior: 'smooth'
                });
            }
        }
        // Si ce n'est pas un lien interne, le navigateur gère le lien normalement
    });
});

// Fonction pour détecter le scroll et ajouter des animations
function checkScroll() {
	const elements = document.querySelectorAll('.content, .gallery');
	elements.forEach(element => {
		const elementTop = element.getBoundingClientRect().top;
		const elementVisible = 150;
		
		if (elementTop < window.innerHeight - elementVisible) {
			element.classList.add('active');
		}
	});
}

// Écouteur d'événement pour le scroll
window.addEventListener('scroll', checkScroll);