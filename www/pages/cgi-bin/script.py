# !/usr/bin/python3
import tkinter as tk
import random
import time


# print("<!DOCTYPE html>")
# print()
# print("<head><title>CGI Test</title><link rel=\"stylesheet\" href=\"../../styles.css\"></head>")
# print("<body>")
# print("<h1>Python CGI executed with success !</h1>")
# print("<p>Server has correctly handle .py file</p>")
# print("<a href=\"../../index.html\" id=\"return\">Go Back Home</a>")
# print("</body>")
# print("</html>")

print("""
<!DOCTYPE html>
<html>
<head>
	<title>Python CGI Test</title>
	<link rel="stylesheet" href="../../styles.css">
</head>
<body>
	<h1>Python CGI Test</h1>
	<div id="game">
		<div id="score">Score: 0</div>
		<div id="timer">Time: 30s</div>
		<div id="game-area" style="position:relative; width:300px; height:200px; border:1px solid #ccc;">
			<div id="target"></div>
		</div>
	</div>

	<script>
	const target = document.getElementById('target');
	const scoreEl = document.getElementById('score');
	const timerEl = document.getElementById('timer');
	let score = 0;
	let timeLeft = 30;

	function moveTarget() {
		const gameArea = document.getElementById('game-area');
		const maxX = gameArea.clientWidth - 50;
		const maxY = gameArea.clientHeight - 50;
		
		target.style.left = `${Math.random() * maxX}px`;
		target.style.top = `${Math.random() * maxY}px`;
	}

	function startGame() {
		score = 0;
		timeLeft = 30;
		scoreEl.textContent = `Score: ${score}`;
		timerEl.textContent = `Temps: ${timeLeft}s`;

		target.addEventListener('click', hitTarget);
		moveTarget();

		const timer = setInterval(() => {
			timeLeft--;
			timerEl.textContent = `Temps: ${timeLeft}s`;

			if (timeLeft <= 0) {
				clearInterval(timer);
				alert(`Jeu terminé ! Score: ${score}`);
				target.removeEventListener('click', hitTarget);
			}
		}, 1000);
	}

	function hitTarget() {
		score++;
		scoreEl.textContent = `Score: ${score}`;
		moveTarget();
	}
	startGame();
	</script>
</body>
</html>
""")