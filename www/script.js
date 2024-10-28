const themeToggle = document.getElementById('theme-toggle');
let isDark = false;

themeToggle.addEventListener('click', () => {
	isDark = !isDark;
	document.body.classList.toggle('dark-theme');
	themeToggle.textContent = isDark ? 'Thème clair' : 'Thème sombre';
});

const images = document.querySelectorAll('.gallery img');
images.forEach(img => {
	img.addEventListener('click', () => {
		img.style.transform = 'scale(1.1)';
		setTimeout(() => {
			img.style.transform = 'scale(1)';
		}, 500);
	});
});

document.querySelectorAll('nav a').forEach(anchor => {
	anchor.addEventListener('click', function(e) {
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
	});
});

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

window.addEventListener('scroll', checkScroll);





// Upload

// Sélection des éléments DOM
const dropZone = document.getElementById('dropZone');
const fileInput = document.getElementById('fileInput');
const fileList = document.getElementById('fileList');

// Configuration
const MAX_FILE_SIZE = 10 * 1024 * 1024; // 10MB en octets
const ACCEPTED_TYPES = ['image/jpeg', 'image/png', 'application/pdf'];

// Fonctions utilitaires
function formatFileSize(bytes) {
	if (bytes === 0) return '0 Bytes';
	const k = 1024;
	const sizes = ['Bytes', 'KB', 'MB', 'GB'];
	const i = Math.floor(Math.log(bytes) / Math.log(k));
	return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

function validateFile(file) {
	// Vérification de la taille
	if (file.size > MAX_FILE_SIZE) {
		alert(`Le fichier ${file.name} est trop volumineux. Taille maximum : 10MB`);
		return false;
	}
	
	// Vérification du type
	if (!ACCEPTED_TYPES.includes(file.type)) {
		alert(`Le type de fichier ${file.type} n'est pas accepté. Types acceptés : JPG, PNG, PDF`);
		return false;
	}
	
	return true;
}

function createFileListItem(file) {
	const li = document.createElement('li');
	
	// Création de l'élément HTML pour le fichier
	li.innerHTML = `
		<div class="file-info">
			<span class="file-name">${file.name}</span>
			<span class="file-size">${formatFileSize(file.size)}</span>
		</div>
		<button class="remove-file" data-name="${file.name}">×</button>
	`;
	
	// Ajout du style
	li.style.cssText = `
		display: flex;
		justify-content: space-between;
		align-items: center;
		padding: 10px;
		background: #f8f9fa;
		margin-bottom: 5px;
		border-radius: 5px;
	`;
	
	// Style pour le bouton de suppression
	const removeButton = li.querySelector('.remove-file');
	removeButton.style.cssText = `
		background: #ff4444;
		color: white;
		border: none;
		border-radius: 50%;
		width: 24px;
		height: 24px;
		cursor: pointer;
		display: flex;
		align-items: center;
		justify-content: center;
	`;
	
	// Gestionnaire d'événement pour la suppression
	removeButton.addEventListener('click', () => {
		li.remove();
		updateFileList();
	});
	
	return li;
}

function updateFileList() {
	// Met à jour le message si la liste est vide
	if (fileList.children.length === 0) {
		fileList.innerHTML = '<li style="text-align: center; color: #666;">Aucun fichier sélectionné</li>';
	}
}

// Gestionnaires d'événements pour le drag & drop
dropZone.addEventListener('dragover', (e) => {
	e.preventDefault();
	dropZone.classList.add('dragover');
});

['dragleave', 'drop'].forEach(eventName => {
	dropZone.addEventListener(eventName, (e) => {
		e.preventDefault();
		dropZone.classList.remove('dragover');
	});
});

dropZone.addEventListener('drop', (e) => {
	e.preventDefault();
	const files = Array.from(e.dataTransfer.files);
	handleFiles(files);
});

// Gestionnaire pour l'input file
fileInput.addEventListener('change', (e) => {
	const files = Array.from(e.target.files);
	handleFiles(files);
});

function handleFiles(files) {
	// Vide le message "Aucun fichier sélectionné" s'il existe
	if (fileList.children.length === 1 && fileList.children[0].textContent === 'Aucun fichier sélectionné') {
		fileList.innerHTML = '';
	}
	
	files.forEach(file => {
		if (validateFile(file)) {
			// Vérifie si le fichier n'est pas déjà dans la liste
			const fileExists = Array.from(fileList.children).some(
				li => li.querySelector('.file-name')?.textContent === file.name
			);
			
			if (!fileExists) {
				// Ajoute le fichier à la liste
				const li = createFileListItem(file);
				fileList.appendChild(li);
			} else {
				alert(`Le fichier ${file.name} est déjà dans la liste`);
			}
		}
	});
	
	// Réinitialise l'input pour permettre la sélection du même fichier
	fileInput.value = '';
}

// Initialisation
updateFileList();

// Empêcher le comportement par défaut du navigateur pour les fichiers déposés
window.addEventListener('drop', (e) => {
	e.preventDefault();
});

window.addEventListener('dragover', (e) => {
	e.preventDefault();
});

// Sélection du bouton d'envoi
const uploadButton = document.getElementById('uploadButton');

// Fonction pour mettre à jour l'état du bouton
function updateUploadButton() {
	const hasFiles = fileList.children.length > 0 && 
					fileList.children[0].textContent !== 'Aucun fichier sélectionné';
	uploadButton.disabled = !hasFiles;
}

// Modification de la fonction updateFileList
function updateFileList() {
	if (fileList.children.length === 0) {
		fileList.innerHTML = '<li style="text-align: center; color: #666;">Aucun fichier sélectionné</li>';
	}
	updateUploadButton(); // Mise à jour de l'état du bouton
}

// Modification de la fonction handleFiles
function handleFiles(files) {
	if (fileList.children.length === 1 && fileList.children[0].textContent === 'Aucun fichier sélectionné') {
		fileList.innerHTML = '';
	}
	
	files.forEach(file => {
		if (validateFile(file)) {
			const fileExists = Array.from(fileList.children).some(
				li => li.querySelector('.file-name')?.textContent === file.name
			);
			
			if (!fileExists) {
				const li = createFileListItem(file);
				fileList.appendChild(li);
				updateUploadButton(); // Mise à jour de l'état du bouton
			} else {
				alert(`Le fichier ${file.name} est déjà dans la liste`);
			}
		}
	});
	
	fileInput.value = '';
}

// Fonction pour créer une barre de progression
function createProgressElement() {
	const progressDiv = document.createElement('div');
	progressDiv.className = 'upload-progress';
	progressDiv.innerHTML = '<div class="progress-bar"></div>';
	return progressDiv;
}

// Fonction pour simuler l'envoi des fichiers
function uploadFiles() {
	const files = Array.from(fileList.children)
		.map(li => li.querySelector('.file-name'))
		.filter(Boolean)
		.map(span => span.textContent);

	if (files.length === 0) return;

	// Désactive le bouton et affiche le spinner
	uploadButton.disabled = true;
	const buttonText = uploadButton.querySelector('.button-text');
	const spinner = uploadButton.querySelector('.loading-spinner');
	buttonText.textContent = 'Envoi en cours...';
	spinner.classList.remove('hidden');

	// Crée et ajoute la barre de progression
	const progressElement = createProgressElement();
	document.querySelector('.upload-actions').insertBefore(
		progressElement, 
		uploadButton.nextSibling
	);
	const progressBar = progressElement.querySelector('.progress-bar');

	// Simule une progression
	let progress = 0;
	const interval = setInterval(() => {
		progress += Math.random() * 30;
		if (progress >= 100) {
			progress = 100;
			clearInterval(interval);
			uploadComplete();
		}
		progressBar.style.width = `${progress}%`;
	}, 500);

	// Fonction appelée quand l'upload est terminé
	function uploadComplete() {
		// Mise à jour de l'interface
		buttonText.textContent = 'Envoi terminé !';
		spinner.classList.add('hidden');
		
		// Création du message de succès
		const messageDiv = document.createElement('div');
		messageDiv.className = 'upload-message success';
		messageDiv.textContent = `${files.length} fichier(s) envoyé(s) avec succès !`;
		
		// Ajout du message
		document.querySelector('.upload-actions').appendChild(messageDiv);
		
		// Réinitialisation après 3 secondes
		setTimeout(() => {
			// Supprime la barre de progression et le message
			progressElement.remove();
			messageDiv.remove();
			
			// Réinitialise le bouton
			buttonText.textContent = 'Envoyer les fichiers';
			uploadButton.disabled = false;
			
			// Vide la liste des fichiers
			fileList.innerHTML = '';
			updateFileList();
		}, 3000);
	}
}

// Ajout du gestionnaire d'événement pour le bouton d'envoi
uploadButton.addEventListener('click', uploadFiles);

// Initialisation
updateFileList();