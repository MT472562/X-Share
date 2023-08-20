def calculate_score(is_japanese, is_english_name, is_english_profile, is_japanese_suffix,
                    has_official_mark, has_quoted_video, has_affiliate_link,
                    post_length, is_ff):
    score = 0
    
    if is_japanese:
        if is_english_name:
            score += 10
        if is_english_profile:
            score += 25
        if is_japanese_suffix:
            score -= 15
    
    if has_official_mark:
        score += 50
    if has_quoted_video:
        score += 20
    if has_affiliate_link:
        score += 40
    
    if post_length > 140:
        score += 15
    
    if is_ff:
        score -= 150
    
    return score

# ここに各種条件を設定してください
is_japanese = True
is_english_name = True
is_english_profile = True
is_japanese_suffix = True
has_official_mark = True
has_quoted_video = True
has_affiliate_link = True
post_length = 160
is_ff = True

total_score = calculate_score(is_japanese, is_english_name, is_english_profile, is_japanese_suffix,
                              has_official_mark, has_quoted_video, has_affiliate_link,
                              post_length, is_ff)

print("Total Score:", total_score)
