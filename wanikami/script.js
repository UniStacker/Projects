/* DOM elements */
const lessons_card_div = document.querySelector(".lessons-card");
const lessons_count_span = document.querySelector(".lessons-count");
const lessons_msg_div = document.querySelector(".lessons-msg");
const reviews_card_div = document.querySelector(".reviews-card");
const reviews_count_span = document.querySelector(".reviews-count");
const reviews_msg_div = document.querySelector(".reviews-msg");
const mistakes_msg_div = document.querySelector(".mistakes-msg");

/* Data variables */
let lesson_count = 15;
let review_count = 5;
let mistake_count = 12;

/* Lessons and Reviews card logic */
if (lesson_count > 0) {
    lessons_count_span.textContent = `${lesson_count}`;
    lessons_msg_div.textContent = "We cooked uo these lessons just for you.";
    lessons_card_div.classList.add("active");
} else {
    lessons_count_span.textContent = "0";
    lessons_msg_div.textContent = "No more lessons for today.";
    lessons_card_div.classList.remove("active");
}

if (review_count > 0) {
    reviews_count_span.textContent = `${review_count}`;
    reviews_msg_div.textContent = "Review these items to level them up!";
    reviews_card_div.classList.add("active");
} else {
    reviews_count_span.textContent = "0";
    reviews_msg_div.textContent = "There are no more reviews to do right now.";
    reviews_card_div.classList.remove("active");
}

if (mistake_count > 0) {
    mistakes_msg_div.textContent = "To seek knowledge is as a trial. In errors, wisdom is birthed. Behold, {user}! All your errors from the past 24 hours, sorted by severity. They yearn for your reverence and contemplation.";
} else {
    mistakes_msg_div.textContent = "At this moment, emptiness prevails. When you do your Reviews, all of your mistakes will appear here, for a single day, in this space of reflection.";
}