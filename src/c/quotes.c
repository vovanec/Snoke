/*
 
 The MIT License (MIT)
 
 Copyright © 2016 Volodymyr Kuznetsov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */


#include "quotes.h"
#include <stdlib.h>

const char* SITH_QUOTES[] = {
    "Peace is a lie.\nThere is only passion.\nThrough passion I gain strength.\nThrough strength I gain power.\nThrough power I gain victory.\nThrough victory my chains are broken.\nThe Force shall set me free.\n–The Sith Code",
    "Do you feel it? Do you feel the power of the dark side? The only real power! The only thing worth living for!",
    "Equality is a lie...a myth to appease the masses. Simply look around and you will see the lie for what it is! There are those with power, those with the strength and will to lead. And there are those meant to follow - those incapable of anything but servitude and a meager, worthless existence.",
    "The dark side will devour those who lack the power to control it. It’s a fierce storm of emotion that annihilates anything in its path. It lays waste to the weak and unworthy. But those who are strong can ride the storm winds to unfathomable heights. They can unlock their true potential; they can sever the chains that bind them; they can dominate the world around them. Only those with the power to control the dark side can ever truly be free.\n–Darth Bane",
    "Without strife, your victory has no meaning. Without strife, you do not advance. Without strife, there is only stagnation.\n–Uthar Wynn",
    "I have studied you, and found nothing but weakness.\n–Darth Sion",
    "Evil is a word used by the ignorant and the weak. The dark side is about survival. It's about unleashing your inner power. It glorifies the strength of the individual.",
    "I can use my powers to conjure up your worst nightmares and bring them to life before your eyes. I can drive you mad with fear, shred your sanity, and leave you a raving lunatic for the rest of your life.",
    "This is the way of our Order. An individual may die, but the Sith are eternal.\n–Darth Zannah",
    "It has been said that anyone who knows the ways of the Force can set her- or himself up as a King on any world where only she or he knows the ways of the Force. Any Jedi could do this. But the Jedi, fools that they are, adhere to a religion in which the Force is used only in the service of others. How shortsighted of them. Is that not why they lost the galaxy to the dark side?\n–Darth Sidious, The Weakness of Inferiors",
    "Oh no, my young Jedi. You will find that it is you who are mistaken...about a great many things.\n–Darth Sidious, Return of the Jedi",
    "Everything of significance is the result of conquest.\n–Darth Sidious, Book of Sith",
    "Through us, the powers of chaos are harnessed and exploited. Dark times don't simply emerge, Sidious. Enlightened beings, guiding intelligences manipulate events to bring about a storm that will deliver power into the hands of an elite group willing to make the hard choices the Republic fears to make. Beings may elect their leaders, but the Force has elected us.",
    "You must begin by gaining power over yourself; then another; then a group, an order, a world, a species, a group of species... finally, the galaxy itself.",
    "The power of the dark side is an illness no true Sith would wish to be cured of.\n–Darth Plagueis",
    "Here in my meditation chamber I can see the galaxy in my mind's eye; I can visualize vast armies, powerful fleets, invincible warriors -- and with Sith arts, my imagination can make them real!\n–Naga Sadow",
    "Don't be too proud of this technological terror you've constructed. The ability to destroy a planet is insignificant next to the power of the Force.",
    "I find your lack of faith disturbing.\n–Darth Vader",
    "To we who dwell in the Force, normal life is little more than pretense. Our only actions of significance are those we undertake in service to the dark side.\n–Darth Tenebrous",
    "Wealth and material goods mean nothing to me. I crave only power and purpose. With power, anything you want or need can simply be taken. With purpose, your life has meaning.\n–Darth Cognus",
    "To be a Sith is to taste freedom and to know victory.\n-Yuthura Ban",
    "Fear. Fear attracts the fearful. The strong. The weak. The innocent. The corrupt. Fear. Fear is my ally.\n–Darth Maul",
    "You are no more powerful than you make yourself. If you see yourself as having weakness, you are simply increasing the problem. It's a self-fulfilling prophecy.\n–Lord Mal'kith",
    "We will disperse into the galaxy in small numbers, masking ourselves, hiding amidst the enemy. We will infiltrate governments on every planet. All must be brought down for something new to be created. Together or alone, we will slip onto worlds and strike from the shadows. We will be invisible. We will be patient. All that exists will be torn apart from within. Darth Krayt’s vision of the galaxy remade will be realized. He lives in us. The Sith will prevail.\n–Darth Nihl",
    "Power is our Passion. War is our Way. Darkness is our Destiny. The Force Shall set us Free!\n–The Sith Academy Code",
    "The true test of our philosophy is war; the real measure of our Force mastery is power; the highest truth of Sithism is victory.",
    "To dare to dream of something that has never existed and to make it a reality; to conjure up your deepest fears and darkest hopes; to pursue a vision ruthlessly and without relent; to create the artifacts, ideology and organization of the empires of your mind: this is real black magic, the source of all true power.",
    "No amount of discipline will turn sheep into wolves.\nWe claim all nations, all planets, all stars – they are ours.\n–Darth Imperius",
    "If two Sith enter a room, only one will leave as Master.",
    "There is truth in a lie, and lies in truth!",
    "You'll know you're making real progress on the Sith path when you go from being suicidal to homicidal.\n–Darth Ravenus"};

size_t NUM_OF_QUOTES = (sizeof( SITH_QUOTES ) / sizeof( SITH_QUOTES[ 0 ] ));
