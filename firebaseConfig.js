import { initializeApp } from "firebase/app";
import { getDatabase, ref, onValue, query, limitToLast } from "firebase/database";

const firebaseConfig = {
    apiKey: "AIzaSyDP2IKJUiQwJGEZySs3F3jH88_dm8jiBoo",
    authDomain: "project-45d44.firebaseapp.com",
    databaseURL: "https://project-45d44-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "project-45d44",
    storageBucket: "project-45d44.appspot.com",
    messagingSenderId: "729057768515",
    appId: "1:729057768515:web:8d123be6003285928010f6",
    measurementId: "G-SW8QM2DFRJ"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

export { db, ref, onValue, query, limitToLast };
