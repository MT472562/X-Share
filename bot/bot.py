import discord
import json

with open("token.json", 'r') as file:
    data = json.load(file)
    token = data["token"]

bot = discord.Client(intents=discord.Intents.all())

@bot.event
async def on_ready():
    print('Bot has logged in successfully')

@bot.event
async def on_message(message):
    if bot.user != message.author:
        if message.content == '/hello':
            await message.channel.send(f'こんにちは！{message.author.mention}')

bot.run(token)
